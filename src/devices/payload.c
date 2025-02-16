#include <libmop/payload.h>
#include <libmop/pl_errno.h>
#include <libmop/pl_list.h>
#include <devices/payload.h>
#include <drivers/edc.h>

#include <stdio.h>
#include <string.h>
#include <time.h>

#define PAYLOAD_UNIX_TO_J2000_EPOCH(x) ((x) - 946684800)

static int edc_pl_init(struct payload *pl)
{
	int err = PL_OK;

	edc_config_t *conf = pl->payload_data;

	err = edc_init(conf);

	if (err == 0) {
		struct timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		pl->ctx->active = 1U;
		pl->ctx->ctx_change_ts.tv_sec = ts.tv_sec;
		pl->ctx->ctx_change_ts.tv_nsec = ts.tv_nsec;
		printf("EDC: Sucessfully initialized %s!\n", pl->name);
	} else {
		err = -PL_ERRNO_UNKNOWN;
	}

	return err;
}

static int edc_pl_write_data(struct payload *pl, const uint8_t type,
			     uint8_t *data, uint16_t size)
{
	return -PL_ERRNO_UNSUPPORTED_FN;
}

static int edc_pl_read_data(struct payload *pl, const uint8_t type,
			    uint8_t *data, uint16_t size)
{
	int err = -PL_ERRNO_UNKNOWN;

	edc_config_t *conf = pl->payload_data;

	switch (type) {
	case EDC_FRAME_ID_HK: {
		edc_hk_t hk = { 0 };

		if (size < sizeof(hk))
			return -PL_ERRNO_DATA;

		if (edc_get_hk(conf, &hk) == 0) {
			for (uint8_t i = 0U; i < sizeof(hk); ++i) {
				data[i] = ((uint8_t *)&hk)[i];
			}
			err = PL_OK;
		}
		break;
	}
	case EDC_FRAME_ID_STATE: {
		edc_state_t st = { 0 };

		if (size < sizeof(st))
			return -PL_ERRNO_DATA;

		if (edc_get_state(conf, &st) == 0) {
			for (uint8_t i = 0U; i < sizeof(st); ++i) {
				data[i] = ((uint8_t *)&st)[i];
			}
			err = PL_OK;
		}
		break;
	}
	case EDC_FRAME_ID_PTT: {
		edc_ptt_t ptt = { 0 };

		if (size < sizeof(ptt))
			return -PL_ERRNO_DATA;

		if (edc_get_ptt(conf, &ptt) == 0) {
			for (uint8_t i = 0U; i < sizeof(ptt); ++i) {
				data[i] = ((uint8_t *)&ptt)[i];
			}

			if (edc_pop_ptt_pkg(conf) == 0)
				err = PL_OK;
		}
		break;
	}
	default:
		err = -PL_ERRNO_UNSUPPORTED_FN;
		break;
	}

	return err;
}
static int edc_pl_write_cmd(struct payload *pl, const uint8_t cmd,
			    uint8_t *cmd_args, uint16_t args_size)
{
	edc_config_t *conf = pl->payload_data;
	edc_cmd_t c = { 0 };

	if (args_size < 4)
		return -PL_ERRNO_DATA;

	c.id = cmd;
	c.param = (cmd_args[0] << 24U) | (cmd_args[1] << 16U) |
		  (cmd_args[2] << 8U) | (cmd_args[3]);

	if (edc_write_cmd(conf, c) != 0)
		return -PL_ERRNO_UNKNOWN;

	return PL_OK;
}

static int edc_pl_set_clock(struct payload *pl,
			    const struct payload_timestamp *ts)
{
	edc_config_t *conf = pl->payload_data;

	if (edc_set_rtc_time(conf, PAYLOAD_UNIX_TO_J2000_EPOCH(ts->tv_sec)) !=
	    0)
		return -PL_ERRNO_UNKNOWN;

	return PL_OK;
}

static int edc_pl_get_clock(struct payload *pl, struct payload_timestamp *ts)
{
	return -PL_ERRNO_UNSUPPORTED_FN;
}

static int edc_pl_enable(struct payload *pl)
{
	return -PL_ERRNO_UNSUPPORTED_FN;
}

static int edc_pl_disable(struct payload *pl)
{
	return -PL_ERRNO_UNSUPPORTED_FN;
}

int payload_edc_init(uint8_t edc_id, struct payload *edc,
		     edc_config_t *edc_conf, struct payload_ctx *edc_ctx)
{
	int err = PL_OK;

	switch (edc_id) {
	case 1U:
		// TODO: Should be rename after Vivado design.
		memset(edc_conf, 0U, sizeof(*edc_conf));
		(void)strncpy(edc_conf->i2c_dev, "/dev/i2c-1", 24U);
		edc_conf->interface = EDC_IF_I2C;
		edc->payload_data = edc_conf;
		edc->ctx = edc_ctx;
		(void)strncpy(edc->name, "EDC1", PAYLOAD_NAME_MAX);
		edc->id = edc_id;
		edc->init = edc_pl_init;
		edc->write_cmd = edc_pl_write_cmd;
		edc->write_data = edc_pl_write_data;
		edc->read_data = edc_pl_read_data;
		edc->disable = edc_pl_disable;
		edc->enable = edc_pl_enable;
		edc->get_clock = edc_pl_get_clock;
		edc->set_clock = edc_pl_set_clock;
		pl_list_add(edc);
		break;
	case 2U:
		err = -PL_ERRNO_UNSUPPORTED_FN;
		break;
	default:
		err = -PL_ERRNO_INVALID_ARG;
		break;
	}

	if (err != 0)
		printf("PAYLOAD: Failed to initialize EDC payload handle.\n");

	return err;
}
