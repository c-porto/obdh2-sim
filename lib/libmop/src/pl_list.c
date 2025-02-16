#include <libmop/pl_errno.h>
#include <libmop/pl_list.h>
#include <libmop/payload.h>

#include <string.h>

/* Payload lists instance */
static struct payload *payload_list = NULL;

#define for_each_payload(handle)                        \
	for ((handle) = payload_list; (handle) != NULL; \
	     (handle) = (handle)->next)

void pl_list_add(struct payload *pl)
{
	if (pl == NULL)
		return;

	pl->next = NULL;

	if (payload_list != NULL) {
		struct payload *last = NULL;
		struct payload *handle = NULL;
		for_each_payload(handle)
		{
			if ((handle == pl) ||
			    (strncmp(pl->name, handle->name,
				     PAYLOAD_NAME_MAX) == 0) ||
			    (pl->id == handle->id)) {
				return;
			}
			last = handle;
		}

		last->next = pl;
	} else {
		payload_list = pl;
	}
}

void pl_list_remove(struct payload *pl)
{
	if (pl == NULL)
		return;

	if (pl == payload_list) {
		payload_list = pl->next;
		pl->next = NULL;
	} else {
		struct payload *cur = NULL;
		for_each_payload(cur)
		{
			if (cur->next == pl) {
				cur->next = pl->next;
				pl->next = NULL;
				break;
			}
		}
	}
}

struct payload *pl_list_get_by_name(const char *name)
{
	struct payload *pl = NULL;

	for_each_payload(pl)
	{
		if (strncmp(pl->name, name, PAYLOAD_NAME_MAX) == 0)
			return pl;
	}

	return NULL;
}

struct payload *pl_list_get_by_id(const uint8_t id)
{
	struct payload *pl = NULL;

	for_each_payload(pl)
	{
		if (pl->id == id)
			return pl;
	}

	return NULL;
}

struct payload *pl_list_get(void)
{
	return payload_list;
}
