#include <pthread.h>
#include <math.h>

#include <libmop/payload.h>

#include <system/sys_log.h>
#include <devices/payload.h>
#include <drivers/edc.h>
#include <time.h>

static void edc_print_hk(struct payload *edc, edc_hk_t *hk)
{
	sys_log_print_event_from_module(SYS_LOG_INFO, edc->name,
					"Elapsed Time: %lu sec",
					hk->elapsed_time);
	sys_log_print_event_from_module(SYS_LOG_INFO, edc->name,
					"Analog current: %u mA",
					hk->current_supply_a);
	sys_log_print_event_from_module(SYS_LOG_INFO, edc->name,
					"Digital current: %u mA",
					hk->current_supply_d);
	sys_log_print_event_from_module(SYS_LOG_INFO, edc->name,
					"System Voltage: %u mV",
					hk->voltage_supply);
	sys_log_print_event_from_module(SYS_LOG_INFO, edc->name,
					"Temperature: %i oC", hk->temp);
	sys_log_print_event_from_module(SYS_LOG_INFO, edc->name,
					"System Voltage: %lu mV",
					hk->voltage_supply);
	sys_log_print_event_from_module(SYS_LOG_INFO, edc->name,
					"RX Counter: %u pkts", hk->num_rx_ptt);
}

static void edc_print_state(struct payload *edc, edc_state_t *state)
{
	sys_log_print_event_from_module(SYS_LOG_INFO, edc->name,
					"Current Time: %lu sec",
					state->current_time);
	sys_log_print_event_from_module(SYS_LOG_INFO, edc->name,
					"PTT Available: %u pkts",
					state->ptt_available);
	sys_log_print_event_from_module(SYS_LOG_INFO, edc->name,
					"PTT Paused: %u", state->ptt_is_paused);
}

static void edc_print_ptt(struct payload *edc, edc_ptt_t *ptt)
{
	int32_t ptt_power = -67 + (20 * log10(ptt->carrier_abs / 32768.0));

	sys_log_print_event_from_module(SYS_LOG_INFO, edc->name,
					"Time Tag: %lu sec", ptt->time_tag);
	sys_log_print_event_from_module(SYS_LOG_INFO, edc->name,
					"Signal Power: %li dBm", ptt_power);
	sys_log_print_event_from_module(SYS_LOG_INFO, edc->name,
					"Carrier Freq: %li kHz",
					ptt->carrier_freq);
}

void *read_edc_thread(void *arg)
{
	(void)arg;

	struct timespec next = { 0 };
	struct payload edc = { 0 };
	struct payload_ctx edc_ctx = { 0 };
	edc_config_t edc_conf;
	edc_hk_t hk;
	edc_state_t state;
	edc_ptt_t ptt;

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

		struct payload_timestamp ts = {
			.tv_sec = next.tv_sec - 60U,
			.tv_nsec = next.tv_nsec,
		};

		if (payload_set_clock(&edc, &ts) != 0) {
			sys_log_print_event_from_module(SYS_LOG_ERROR, "edc",
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

		if (payload_read_data(&edc, EDC_FRAME_ID_HK, (uint8_t *)&hk,
				      sizeof(hk)) == 0) {
			edc_print_hk(&edc, &hk);
		} else {
			sys_log_print_event_from_module(SYS_LOG_ERROR, edc.name,
							"Failed to read hk!");
		}

		edc_delay_ms(500U);

		if (payload_read_data(&edc, EDC_FRAME_ID_STATE,
				      (uint8_t *)&state, sizeof(state)) == 0) {
			edc_print_state(&edc, &state);

			if (state.ptt_available > 0) {
				for (uint8_t i = 0; i < state.ptt_available;
				     i++) {
					if (payload_read_data(
						    &edc, EDC_FRAME_ID_PTT,
						    (uint8_t *)&ptt,
						    sizeof(ptt)) == 0) {
						edc_print_ptt(&edc, &ptt);
					} else {
						sys_log_print_event_from_module(
							SYS_LOG_ERROR, edc.name,
							"Error reading ptt package!");
					}

					edc_delay_ms(500U);
				}
			}
		} else {
			sys_log_print_event_from_module(SYS_LOG_ERROR, edc.name,
							"Error reading state!");
		}

		return NULL;
	}
}
