#include <pthread.h>
#include <math.h>

#include <libmop/payload.h>
#include <libmop/pl_errno.h>

#include <system/context.h>
#include <system/sys_log.h>
#include <system/db.h>
#include <devices/payload.h>
#include <drivers/edc.h>
#include <time.h>

static void edc_save_hk(struct db_handle *db, struct payload *edc, edc_hk_t *hk)
{
	if (db->handle) {
		tm_db_add_entry(db, edc->name, STRINGZ(elapsed_time),
				(double)hk->elapsed_time);
		tm_db_add_entry(db, edc->name, STRINGZ(current_supply_a),
				(double)hk->current_supply_a);
		tm_db_add_entry(db, edc->name, STRINGZ(current_supply_d),
				(double)hk->current_supply_d);
		tm_db_add_entry(db, edc->name, STRINGZ(voltage_supply),
				(double)hk->voltage_supply);
		tm_db_add_entry(db, edc->name, STRINGZ(temp), (double)hk->temp);
		tm_db_add_entry(db, edc->name, STRINGZ(temp),
				(double)hk->num_rx_ptt);
	}
}

static void edc_save_state(struct db_handle *db, struct payload *edc,
			   edc_state_t *state)
{
	if (db->handle) {
		tm_db_add_entry(db, edc->name, STRINGZ(current_time),
				(double)state->current_time);
		tm_db_add_entry(db, edc->name, STRINGZ(ptt_available),
				(double)state->ptt_available);
		tm_db_add_entry(db, edc->name, STRINGZ(ptt_is_paused),
				(double)state->ptt_is_paused);
	}
}

static void edc_save_ptt(struct db_handle *db, struct payload *edc,
			 edc_ptt_t *ptt)
{
	int32_t ptt_power = -67 + (20 * log10(ptt->carrier_abs / 32768.0));

	if (db->handle) {
		tm_db_add_entry(db, edc->name, STRINGZ(time_tag),
				(double)ptt->time_tag);
		tm_db_add_entry(db, edc->name, STRINGZ(ptt_power),
				(double)ptt_power);
		tm_db_add_entry(db, edc->name, STRINGZ(ptt_carrier_freq),
				(double)ptt->carrier_freq);
	}
}

void *read_edc_thread(void *arg)
{
	struct obdh_sim_ctx *ctx = arg;

	struct timespec next = { 0 };
	struct timespec now = { 0 };
	struct payload edc = { 0 };
	struct payload_ctx edc_ctx = { 0 };
	edc_config_t edc_conf;
	edc_hk_t hk;
	edc_state_t state;
	edc_ptt_t ptt;

	struct db_handle db = { 0 };
	const char *db_file = "/var/local/edc.sqlite3";

	if (create_tm_db(&db, db_file) < 0)
		sys_log_print_event_from_module(SYS_LOG_ERROR, "edc",
						"Failed to create DB!");

	edc_conf.en_pin = 1U;

	if (payload_edc_init(1U, &edc, &edc_conf, &edc_ctx) != 0) {
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, "edc",
			"Failed to initialize EDC context!");
	}

	if (payload_init(&edc) != 0) {
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, "edc",
			"Failed to initialize EDC payload!");
	}

	clock_gettime(CLOCK_MONOTONIC, &next);

	for (;;) {
		next.tv_sec += 60;

		if (!ctx->cond.in_brazil) {
			if (edc.ctx->active) {
				sys_log_print_event_from_module(
					SYS_LOG_INFO, "edc",
					"Disabling EDC...");

				if (payload_disable(&edc) != PL_OK) {
					sys_log_print_event_from_module(
						SYS_LOG_ERROR, "edc",
						"Failed to disabled edc!");
				}
			}
		}

		if (ctx->cond.in_brazil) {
			if (!edc.ctx->active) {
				sys_log_print_event_from_module(
					SYS_LOG_INFO, "edc", "Enabling EDC...");

				if (payload_enable(&edc) != PL_OK) {
					sys_log_print_event_from_module(
						SYS_LOG_ERROR, "edc",
						"Failed to enable edc!");
				}
			}

			clock_gettime(CLOCK_MONOTONIC, &now);

			struct payload_timestamp ts = {
				.tv_sec = now.tv_sec,
				.tv_nsec = now.tv_nsec,
			};

			if (payload_set_clock(&edc, &ts) != 0) {
				sys_log_print_event_from_module(
					SYS_LOG_ERROR, "edc",
					"Failed to set clock!");
			}

			uint8_t cmd[4] = { 0 };
			if (payload_write_cmd(&edc, EDC_CMD_PTT_RESUME, cmd,
					      sizeof(cmd)) != 0) {
				sys_log_print_event_from_module(
					SYS_LOG_ERROR, "edc",
					"Failed to resume ptt task!");
			}

			edc_delay_ms(50U);

			if (payload_read_data(&edc, EDC_FRAME_ID_HK,
					      (uint8_t *)&hk,
					      sizeof(hk)) == 0) {
				edc_save_hk(&db, &edc, &hk);
			} else {
				sys_log_print_event_from_module(
					SYS_LOG_ERROR, edc.name,
					"Failed to read hk!");
			}

			edc_delay_ms(500U);

			if (payload_read_data(&edc, EDC_FRAME_ID_STATE,
					      (uint8_t *)&state,
					      sizeof(state)) == 0) {
				edc_save_state(&db, &edc, &state);

				if (state.ptt_available > 0) {
					for (uint8_t i = 0;
					     i < state.ptt_available; i++) {
						if (payload_read_data(
							    &edc,
							    EDC_FRAME_ID_PTT,
							    (uint8_t *)&ptt,
							    sizeof(ptt)) == 0) {
							edc_save_ptt(&db, &edc,
								     &ptt);
						} else {
							sys_log_print_event_from_module(
								SYS_LOG_ERROR,
								edc.name,
								"Error reading ptt package!");
						}

						edc_delay_ms(500U);
					}
				}
			} else {
				sys_log_print_event_from_module(
					SYS_LOG_ERROR, edc.name,
					"Error reading state!");
			}
		}

		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
	}

	return NULL;
}
