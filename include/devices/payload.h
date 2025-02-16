#ifndef DEV_PAYLOAD_H_
#define DEV_PAYLOAD_H_

#include <drivers/edc.h>
#include <libmop/payload.h>

/**
 * @brief Populates and initializes the EDC payload using libmop structure,
 * also includes it to the pl_list.
 *
 * @param[in] edc_id is the EDC device ID, include since there can be more than 
 * one per mission.
 *
 * @param[in] edc is the EDC payload handle.
 *
 * @param[in] edc_conf is the EDC driver configuration structure that becomes
 * the payload_data of the payload handle.
 *
 * @param[in] edc_ctx is the EDC payload context instance to include in the
 * payload handle.
 *
 * @return Error code from pl_errno enum.
 */
int payload_edc_init(uint8_t edc_id, struct payload *edc, edc_config_t *edc_conf,
                     struct payload_ctx *edc_ctx);

#endif
