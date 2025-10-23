#include <pthread.h>
#include <stdint.h>

#include <stdlib.h>
#include <system/sys_log.h>
#include <system/context.h>

extern void *pos_det_thread(void *arg);
extern void *read_ttc_thread(void *arg);
extern void *read_eps_thread(void *arg);
extern void *read_edc_thread(void *arg);
extern void *control_heater_thread(void *arg);

int main(void)
{
	struct obdh_sim_ctx ctx = { 0 };
	ctx.tids = calloc(5U, sizeof(pthread_t));

	if (pthread_mutex_init(&ctx.lock, NULL) < 0) {
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, "ctx",
			"Failed to initialize mutex! Exiting...");
		exit(1);
	}

	pthread_create(&ctx.tids[0], NULL, pos_det_thread, (void *)&ctx);
	pthread_create(&ctx.tids[1], NULL, read_ttc_thread, (void *)&ctx);
	pthread_create(&ctx.tids[2], NULL, read_eps_thread, (void *)&ctx);
	pthread_create(&ctx.tids[3], NULL, read_edc_thread, (void *)&ctx);
	pthread_create(&ctx.tids[4], NULL, control_heater_thread, (void *)&ctx);

	for (uint8_t i = 0U; i < 5U; ++i) {
		pthread_join(ctx.tids[i], NULL);
	}
}
