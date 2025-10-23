#ifndef SIM_CONTEXT_H_
#define SIM_CONTEXT_H_

#include <stdint.h>
#include <pthread.h>

struct obdh_sim_ctx {
	pthread_mutex_t lock;
	pthread_t *tids;
	union {
		struct {
			uint8_t reserved : 7;
			uint8_t eclipsed : 1;
		};
		uint8_t ext;
	} cond;
};

#endif
