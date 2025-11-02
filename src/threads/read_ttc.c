#include <pthread.h>

#include <system/db.h>
#include <system/sys_log.h>
#include <devices/ttc.h>
#include <devices/ttc_data.h>

static void ttc_save_data(struct db_handle *db, ttc_data_t *data,
			  const char *ttc)
{
	if (db->handle) {
		tm_db_add_entry(db, ttc, STRINGZ(uc_temp),
				(double)data->temperature_mcu);
		tm_db_add_entry(db, ttc, STRINGZ(temperature_radio),
				(double)data->temperature_radio);
		tm_db_add_entry(db, ttc, STRINGZ(current_mcu),
				(double)data->current_mcu);
		tm_db_add_entry(db, ttc, STRINGZ(current_radio),
				(double)data->current_radio);
		tm_db_add_entry(db, ttc, STRINGZ(voltage_mcu),
				(double)data->voltage_mcu);
		tm_db_add_entry(db, ttc, STRINGZ(voltage_radio),
				(double)data->voltage_radio);
		tm_db_add_entry(db, ttc, STRINGZ(rx_packets),
				(double)data->rx_packet_counter);
		tm_db_add_entry(db, ttc, STRINGZ(tx_packets),
				(double)data->tx_packet_counter);
	}
}

void *read_ttc_thread(void *arg)
{
	(void)arg;

	struct timespec next = { 0 };
	ttc_data_t ttc0_data;
	ttc_data_t ttc1_data;

	struct db_handle db = { 0 };
	const char *db_file = "/var/local/ttc.sqlite3";

	if (create_tm_db(&db, db_file) < 0)
		sys_log_print_event_from_module(SYS_LOG_ERROR, "eps",
						"Failed to create DB!");

	clock_gettime(CLOCK_MONOTONIC, &next);

	for (;;) {
		next.tv_sec += 60;

		if (ttc_init(TTC_0) != 0) {
			sys_log_print_event_from_module(
				SYS_LOG_ERROR, "ReadTTC",
				"Error initializing the TTC device!");
		}

		if (ttc_init(TTC_1) != 0) {
			sys_log_print_event_from_module(
				SYS_LOG_ERROR, "ReadTTC",
				"Error initializing the TTC device!");
		}

		if (ttc_get_data(TTC_0, &ttc0_data) != 0) {
			sys_log_print_event_from_module(
				SYS_LOG_ERROR, "ReadTTC",
				"Error reading data from the TTC 0 device!");
		} else {
			ttc_save_data(&db, &ttc0_data, "ttc0");
		}

		if (ttc_get_data(TTC_1, &ttc1_data) != 0) {
			sys_log_print_event_from_module(
				SYS_LOG_ERROR, "ReadTTC",
				"Error reading data from the TTC 1 device!");
		} else {
			ttc_save_data(&db, &ttc1_data, "ttc1");
		}

		/* Checks if there was too many decoding errors on TTC */
		if (ttc_check_failed_pkts(TTC_0) != 0) {
			sys_log_print_event_from_module(
				SYS_LOG_ERROR, "ReadTTC",
				"Error checking for decode errors from TTC 0 device!");
		}

		if (ttc_check_failed_pkts(TTC_1) != 0) {
			sys_log_print_event_from_module(
				SYS_LOG_ERROR, "ReadTTC",
				"Error checking for decode errors from TTC 1 device!");
		}

		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
	};

	return NULL;
}
