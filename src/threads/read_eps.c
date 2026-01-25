#include <pthread.h>
#include <stdint.h>

#include <system/context.h>
#include <system/sys_log.h>
#include <system/db.h>
#include <devices/eps.h>
#include <drivers/sl_eps2.h>

#define READ_EPS_MAX_RETRIES 5U

static void eps_save_data(struct db_handle *db, eps_data_t *data)
{
	int16_t tmp;

	if (db->handle) {
		tm_db_add_entry(db, "eps", STRINGZ(uc_temp),
				(double)data->temperature_uc);
		tm_db_add_entry(db, "eps", STRINGZ(solar_panel_voltage_my_px),
				(double)data->solar_panel_voltage_my_px);
		tm_db_add_entry(db, "eps", STRINGZ(solar_panel_voltage_mx_pz),
				(double)data->solar_panel_voltage_mx_pz);
		tm_db_add_entry(db, "eps", STRINGZ(solar_panel_voltage_mz_py),
				(double)data->solar_panel_voltage_mz_py);
		tm_db_add_entry(db, "eps", STRINGZ(solar_panel_current_px),
				(double)data->solar_panel_current_px);
		tm_db_add_entry(db, "eps", STRINGZ(solar_panel_current_py),
				(double)data->solar_panel_current_py);
		tm_db_add_entry(db, "eps", STRINGZ(solar_panel_current_pz),
				(double)data->solar_panel_current_pz);
		tm_db_add_entry(db, "eps", STRINGZ(solar_panel_current_mx),
				(double)data->solar_panel_current_mx);
		tm_db_add_entry(db, "eps", STRINGZ(solar_panel_current_my),
				(double)data->solar_panel_current_my);
		tm_db_add_entry(db, "eps", STRINGZ(solar_panel_current_mz),
				(double)data->solar_panel_current_mz);
		tm_db_add_entry(db, "eps", STRINGZ(solar_panel_output_voltage),
				(double)data->solar_panel_output_voltage);
		tm_db_add_entry(db, "eps", STRINGZ(main_power_bus_voltage),
				(double)data->main_power_bus_voltage);
		tm_db_add_entry(db, "eps", STRINGZ(rtd_0_temperature),
				(double)data->rtd_0_temperature);
		tm_db_add_entry(db, "eps", STRINGZ(rtd_1_temperature),
				(double)data->rtd_1_temperature);
		tm_db_add_entry(db, "eps", STRINGZ(rtd_2_temperature),
				(double)data->rtd_2_temperature);
		tm_db_add_entry(db, "eps", STRINGZ(rtd_3_temperature),
				(double)data->rtd_3_temperature);
		tm_db_add_entry(db, "eps", STRINGZ(rtd_4_temperature),
				(double)data->rtd_4_temperature);
		tm_db_add_entry(db, "eps", STRINGZ(rtd_5_temperature),
				(double)data->rtd_5_temperature);
		tm_db_add_entry(db, "eps", STRINGZ(rtd_6_temperature),
				(double)data->rtd_6_temperature);
		tm_db_add_entry(db, "eps", STRINGZ(battery_voltage),
				(double)data->battery_voltage);
		tm_db_add_entry(db, "eps", STRINGZ(battery_acc_current),
				(double)data->battery_acc_current);
		tm_db_add_entry(db, "eps", STRINGZ(battery_monitor_temperature),
				(double)data->battery_monitor_temperature);
		tm_db_add_entry(db, "eps", STRINGZ(battery_heater_1_duty_cycle),
				(double)data->battery_heater_1_duty_cycle);
		tm_db_add_entry(db, "eps", STRINGZ(battery_heater_2_duty_cycle),
				(double)data->battery_heater_2_duty_cycle);
		tm_db_add_entry(db, "eps", STRINGZ(battery_heater_1_mode),
				(double)data->battery_heater_1_mode);
		tm_db_add_entry(db, "eps", STRINGZ(battery_heater_2_mode),
				(double)data->battery_heater_2_mode);

		tmp = (int16_t)data->battery_current;
		tm_db_add_entry(db, "eps", STRINGZ(battery_current),
				(double)tmp);

		tmp = (int16_t)data->battery_average_current;
		tm_db_add_entry(db, "eps", STRINGZ(battery_average_current),
				(double)tmp);
	}
}

void *read_eps_thread(void *arg)
{
	struct obdh_sim_ctx *ctx = arg;

	struct timespec next = { 0 };
	eps_data_t eps_data = { 0 };

	struct db_handle db = { 0 };
	const char *db_file = "/var/local/eps.sqlite3";

	if (create_tm_db(&db, db_file) < 0)
		sys_log_print_event_from_module(SYS_LOG_ERROR, "eps",
						"Failed to create DB!");

	clock_gettime(CLOCK_MONOTONIC, &next);

	for (;;) {
		next.tv_sec += 60;

		int8_t err = 0;

		pthread_mutex_lock(&ctx->lock);

		err = eps_init();

		pthread_mutex_unlock(&ctx->lock);

		if (err != 0U) {
			sys_log_print_event_from_module(
				SYS_LOG_ERROR, "eps",
				"Failed when trying to initialize EPS!!!");
		}

		err = 0;

		pthread_mutex_lock(&ctx->lock);

		err = eps_get_data(&eps_data);

		pthread_mutex_unlock(&ctx->lock);

		eps_save_data(&db, &eps_data);

		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
	}

	return NULL;
}
