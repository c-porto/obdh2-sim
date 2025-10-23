#include <pthread.h>

#include <system/sys_log.h>
#include <devices/eps.h>
#include <drivers/sl_eps2.h>

#define READ_EPS_MAX_RETRIES 5U

void *read_eps_thread(void *arg)
{
	(void)arg;

	struct timespec next = { 0 };
	eps_data_t eps_data;

	clock_gettime(CLOCK_MONOTONIC, &next);

	for (;;) {
		next.tv_sec += 10;
		int8_t err = 0;
		uint8_t retry_count = READ_EPS_MAX_RETRIES;

		do {
			err = eps_init();

			if (err != 0) {
				retry_count--;
				sl_eps2_delay_ms(100U);
			}
		} while ((err != 0) && (retry_count > 0U));

		if (retry_count == 0U) {
			sys_log_print_event_from_module(
				SYS_LOG_ERROR, "eps",
				"Max retries reached trying to initialize EPS!!!");
		}

		err = 0;
		retry_count = READ_EPS_MAX_RETRIES;

		do {
			err = eps_get_data(&eps_data);

			if (err != 0) {
				retry_count--;
				sl_eps2_delay_ms(100U);
			}
		} while ((err != 0) && (retry_count > 0U));

		eps_print_data(&eps_data);

		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
	}

	return NULL;
}
