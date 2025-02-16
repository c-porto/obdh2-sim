#ifndef LIBMOP_PL_LIST_H_
#define LIBMOP_PL_LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <libmop/payload.h>

/**
 * @brief Adds a payload handle to the payloads list.
 *
 * @param[in] pl is a payload handle to add to the list.
 */
void pl_list_add(struct payload *pl);

/**
 * @brief Removes a payload handle from the payloads list.
 *
 * @param[in] pl is a payload handle to remove from the list.
 */
void pl_list_remove(struct payload *pl);

/**
 * @brief Tries to get the payload handle that have the matching name from the 
 * payloads list.
 *
 * @param[in] name is the desired payload name.
 *
 * @return The specified payload handle if found, NULL otherwise.
 */
struct payload *pl_list_get_by_name(const char *name);

/**
 * @brief Tries to get the payload handle that have the matching id from the 
 * payloads list.
 *
 * @param[in] id is the desired payload id.
 *
 * @return The specified payload handle if found, NULL otherwise.
 */
struct payload *pl_list_get_by_id(const uint8_t id);

/**
 * @brief Gets head of the payload list.
 *
 * @return The head of the payload list.
 */
struct payload *pl_list_get(void);

#ifdef __cplusplus
}
#endif

#endif
