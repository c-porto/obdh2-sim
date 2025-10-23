#include <pthread.h>

#include <system/context.h>
#include <system/sys_log.h>

#include <devices/eps.h>
#include <drivers/sl_eps2.h>

void *control_heater_thread(void *arg)
{
	struct obdh_sim_ctx *ctx = arg;
	struct timespec next = { 0 };

	clock_gettime(CLOCK_MONOTONIC, &next);

	for (;;) {
		next.tv_sec += 10;

		if (ctx->cond.eclipsed) {
			sys_log_print_event_from_module(
				SYS_LOG_INFO, "heater",
				"Satellite is eclipsed! Enabling heaters...");

			if (eps_set_param(SL_EPS2_REG_BAT_HEATER_1_MODE,
					  SL_EPS2_HEATER_MODE_MANUAL) < 0) {
				sys_log_print_event_from_module(
					SYS_LOG_ERROR, "heater",
					"Failed to set Heater 1 to manual!");
			}

			if (eps_set_param(SL_EPS2_REG_BAT_HEATER_2_MODE,
					  SL_EPS2_HEATER_MODE_MANUAL) < 0) {
				sys_log_print_event_from_module(
					SYS_LOG_ERROR, "heater",
					"Failed to set Heater 2 to manual!");
			}

			if (eps_set_param(SL_EPS2_REG_BAT_HEATER_1_DUTY_CYCLE,
					  100U) < 0) {
				sys_log_print_event_from_module(
					SYS_LOG_ERROR, "heater",
					"Failed to set Heater 1 duty to 100%!");
			}

			if (eps_set_param(SL_EPS2_REG_BAT_HEATER_2_DUTY_CYCLE,
					  100U) < 0) {
				sys_log_print_event_from_module(
					SYS_LOG_ERROR, "heater",
					"Failed to set Heater 2 duty to 100%!");
			}
		} else {
			sys_log_print_event_from_module(
				SYS_LOG_INFO, "heater",
				"Satellite is not eclipsed! Disabling heaters...");

			if (eps_set_param(SL_EPS2_REG_BAT_HEATER_1_DUTY_CYCLE,
					  0U) < 0) {
				sys_log_print_event_from_module(
					SYS_LOG_ERROR, "heater",
					"Failed to set Heater 1 duty to 0%!");
			}

			if (eps_set_param(SL_EPS2_REG_BAT_HEATER_2_DUTY_CYCLE,
					  0U) < 0) {
				sys_log_print_event_from_module(
					SYS_LOG_ERROR, "heater",
					"Failed to set Heater 2 duty to 0%!");
			}
		}

		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
	};

	return NULL;
}
