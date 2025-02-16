#ifndef LIBMOP_PAYLOAD_H_
#define LIBMOP_PAYLOAD_H_

#include <stdint.h>
#include <stddef.h>

#include "pl_errno.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PAYLOAD_NAME_MAX 15U

/**
 * @brief Payload Timestamp structure, used to store time information for 
 * payloads.
 */
struct payload_timestamp {
	uint32_t tv_sec;
	uint32_t tv_nsec;
};

/**
 * @brief Payload Context structure, used to monitor payload state. Should be 
 * saved in some kind of storage to ensure payload state is consistent across 
 * resets.
 */
struct payload_ctx {
	uint8_t active;
	struct payload_timestamp ctx_change_ts;
	/// Probably more state things will be added
};

/**
 * @brief Main Payload structure, includes all information for a given payload,
 * including its driver specific interface, done with function pointers. Could 
 * also be include into the pl_list to facilitate a centralized payload control.
 */
struct payload {
	struct payload_ctx *ctx;
	uint8_t id;
	char name[PAYLOAD_NAME_MAX];
	void *payload_data;
	int (*init)(struct payload *pl);
	int (*write_data)(struct payload *pl, const uint8_t type, uint8_t *data,
			  uint16_t size);
	int (*read_data)(struct payload *pl, const uint8_t type, uint8_t *data,
			 uint16_t size);
	int (*write_cmd)(struct payload *pl, const uint8_t cmd,
			 uint8_t *cmd_args, uint16_t args_size);
	int (*set_clock)(struct payload *pl,
			 const struct payload_timestamp *ts);
	int (*get_clock)(struct payload *pl, struct payload_timestamp *ts);
	int (*enable)(struct payload *pl);
	int (*disable)(struct payload *pl);
	struct payload *next; /**< Used for pl_list */
};

/**
 * @brief Registers the payload specific context to a payload handle.
 *
 * @param[in] pl is a payload handle.
 *
 * @param[in] pl_data is the payload specific data to pass to the payload structure,
 * which could be used for the payload driver.
 *
 * @return Error code from pl_errno enum.
 */
static inline int register_payload_data(struct payload *pl, void *pl_data)
{
	int err = PL_OK;

	if ((pl != NULL) && (pl_data != NULL))
		pl->payload_data = pl_data;
	else
		err = PL_ERRNO_INVALID_ARG;

	return err;
}

/**
 * @brief Registers the payload specific data to a payload handle.
 *
 * @param[in] pl is a payload handle.
 *
 * @param[in] ctx is the payload specific context to pass to the payload structure,
 * which could be used for the payload driver.
 *
 * @return Error code from pl_errno enum.
 */
static inline int register_payload_context(struct payload *pl,
					   struct payload_ctx *ctx)
{
	int err = PL_OK;

	if ((pl != NULL) && (ctx != NULL))
		pl->ctx = ctx;
	else
		err = PL_ERRNO_INVALID_ARG;

	return err;
}

/**
 * @brief Initializes payload. Basically calls the payload specific function.
 * IMPORTANT! If the specific payload function leaves the payload active it 
 * should change the context accordingly.
 *
 * @param[in] pl is a payload handle to initialize. IMPORTANT! This function 
 * assumes that the necessary function pointers, payload data and context were
 * registered.
 *
 * @return Error code from pl_errno enum.
 */
int payload_init(struct payload *pl);

/**
 * @brief Writes data to the specified payload. Basically calls the payload 
 * specific function.
 *
 * @param[in] pl is a payload handle. IMPORTANT! This function assumes that the
 * necessary function pointers, payload data and context were registered.
 *
 * @param[in] type is the type of data to be written to payload, which could be 
 * used by the payload driver.
 *
 * @param[in] data is the data buffer that contains the data to be written.
 *
 * @param[in] size the data buffer size.
 *
 * @return Error code from pl_errno enum.
 */
int payload_write_data(struct payload *pl, const uint8_t type, uint8_t *data,
		       uint16_t size);

/**
 * @brief Reads data to the specified payload. Basically calls the payload 
 * specific function.
 *
 * @param[in] pl is a payload handle. IMPORTANT! This function assumes that the
 * necessary function pointers, payload data and context were registered.
 *
 * @param[in] type is the type of data to be read from the payload, which could 
 * be used by the payload driver.
 *
 * @param[out] data is the data buffer that will store the data read from the
 * payload.
 *
 * @param[in] size the data buffer size.
 *
 * @return Error code from pl_errno enum.
 */
int payload_read_data(struct payload *pl, const uint8_t type, uint8_t *data,
		      uint16_t size);

/**
 * @brief Writes a command to the specified payload. Basically calls the 
 * payload specific function.
 *
 * @param[in] pl is a payload handle. IMPORTANT! This function assumes that the
 * necessary function pointers, payload data and context were registered.
 *
 * @param[in] cmd is the command id to be sent to the specified payload, which 
 * is gonna be used by the payload driver.
 *
 * @param[in] cmd_args is a data buffer that will store arguments to be passed 
 * the payload driver.
 *
 * @param[in] args_size the cmd_args buffer size.
 *
 * @return Error code from pl_errno enum.
 */
int payload_write_cmd(struct payload *pl, const uint8_t cmd, uint8_t *cmd_args,
		      uint16_t args_size);

/**
 * @brief Sets the specified payload clock. Basically calls the payload 
 * specific function.
 *
 * @param[in] pl is a payload handle. IMPORTANT! This function assumes that the
 * necessary function pointers, payload data and context were registered.
 *
 * @param[out] ts is the timestamp to set the the specified payload clock.
 *
 * @return Error code from pl_errno enum.
 */
int payload_set_clock(struct payload *pl, const struct payload_timestamp *ts);

/**
 * @brief Reads the specified payload clock. Basically calls the payload 
 * specific function.
 *
 * @param[in] pl is a payload handle. IMPORTANT! This function assumes that the
 * necessary function pointers, payload data and context were registered.
 *
 * @param[out] ts is the read timestamp from the payload.
 *
 * @return Error code from pl_errno enum.
 */
int payload_get_clock(struct payload *pl, struct payload_timestamp *ts);

/**
 * @brief Enables the specified payload. IMPORTANT! The driver function should 
 * set the context accordingly.
 *
 * @param[in] pl is a payload handle. IMPORTANT! This function assumes that the
 * necessary function pointers, payload data and context were registered.
 *
 * @return Error code from pl_errno enum.
 */
int payload_enable(struct payload *pl);

/**
 * @brief Disables the specified payload. IMPORTANT! The driver function should 
 * set the context accordingly.
 *
 * @param[in] pl is a payload handle. IMPORTANT! This function assumes that the
 * necessary function pointers, payload data and context were registered.
 *
 * @return Error code from pl_errno enum.
 */
int payload_disable(struct payload *pl);

#ifdef __cplusplus
}
#endif

#endif
