#ifndef PL_ERRNO_H_
#define PL_ERRNO_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum pl_errno {
	PL_OK = 0,
	PL_ERRNO_INVALID_ARG = 1,
	PL_ERRNO_TIMEOUT = 2,
	PL_ERRNO_UNKNOWN = 3,
	PL_ERRNO_UNSUPPORTED_FN = 4,
	PL_ERRNO_IO = 5,
};

#ifdef __cplusplus
}
#endif

#endif
