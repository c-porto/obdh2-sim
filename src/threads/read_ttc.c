#include <pthread.h>

#include <system/sys_log.h>
#include <devices/ttc.h>
#include <devices/ttc_data.h>

void *read_ttc_thread(void *arg)
{
	(void)arg;

	struct timespec next = { 0 };
	ttc_data_t ttc0_data;
	ttc_data_t ttc1_data;

	clock_gettime(CLOCK_MONOTONIC, &next);

	for (;;) {
		next.tv_sec += 10;

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
		}

		if (ttc_get_data(TTC_1, &ttc1_data) != 0) {
			sys_log_print_event_from_module(
				SYS_LOG_ERROR, "ReadTTC",
				"Error reading data from the TTC 1 device!");
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

		ttc_print_data(TTC_0, &ttc0_data);

		ttc_print_data(TTC_1, &ttc1_data);

		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
	};

	return NULL;
}
