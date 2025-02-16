#include <libmop/pl_errno.h>
#include <libmop/payload.h>
#include <stdint.h>

int payload_init(struct payload *pl)
{
	int err = PL_ERRNO_INVALID_ARG;

	if ((pl != NULL) && (pl->init != NULL))
		err = (pl->init)(pl);

	return err;
}

int payload_write_data(struct payload *pl, const uint8_t type, uint8_t *data,
		       uint16_t size)
{
	int err = PL_ERRNO_INVALID_ARG;

	if ((pl != NULL) && (pl->write_data != NULL) && (data != NULL))
		err = (pl->write_data)(pl, type, data, size);

	return err;
}

int payload_read_data(struct payload *pl, const uint8_t type, uint8_t *data,
		      uint16_t size)
{
	int err = PL_ERRNO_INVALID_ARG;

	if ((pl != NULL) && (pl->read_data != NULL) && (data != NULL))
		err = (pl->read_data)(pl, type, data, size);

	return err;
}

int payload_write_cmd(struct payload *pl, const uint8_t cmd, uint8_t *cmd_args,
		      uint16_t args_size)
{
	int err = PL_ERRNO_INVALID_ARG;

	if ((pl != NULL) && (pl->write_cmd != NULL) && (cmd_args != NULL))
		err = (pl->write_cmd)(pl, cmd, cmd_args, args_size);

	return err;
}

int payload_set_clock(struct payload *pl, const struct payload_timestamp *ts)
{
	int err = PL_ERRNO_INVALID_ARG;

	if ((pl != NULL) && (pl->set_clock != NULL) && (ts != NULL))
		err = (pl->set_clock)(pl, ts);

	return err;
}

int payload_get_clock(struct payload *pl, struct payload_timestamp *ts)
{
	int err = PL_ERRNO_INVALID_ARG;

	if ((pl != NULL) && (pl->get_clock != NULL) && (ts != NULL))
		err = (pl->get_clock)(pl, ts);

	return err;
}

int payload_enable(struct payload *pl)
{
	int err = PL_ERRNO_INVALID_ARG;

	if ((pl != NULL) && (pl->enable != NULL))
		err = (pl->enable)(pl);

	return err;
}

int payload_disable(struct payload *pl)
{
	int err = PL_ERRNO_INVALID_ARG;

	if ((pl != NULL) && (pl->disable != NULL))
		err = (pl->disable)(pl);

	return err;
}
