/*
 * ttc.c
 * 
 * Copyright The OBDH 2.0 Contributors.
 * 
 * This file is part of OBDH 2.0.
 * 
 * OBDH 2.0 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * OBDH 2.0 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with OBDH 2.0. If not, see <http:/\/www.gnu.org/licenses/>.
 * 
 */

/**
 * \brief TTC device implementation.
 * 
 * \author Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
 * 
 * \version 1.0.0
 * 
 * \date 2020/02/01
 * 
 * \addtogroup ttc
 * \{
 */

#include <stdbool.h>
#include <string.h>

#include <system/sys_log.h>
#include <devices/ttc.h>

static ttc_config_t ttc_0_config;
static ttc_config_t ttc_1_config;

int ttc_init(ttc_e dev)
{
	static bool ttc_0_is_open = false;
	static bool ttc_1_is_open = false;

	int err = -2;

	ttc_config_t ttc_config = { 0 };

	switch (dev) {
	case TTC_0:
		if (ttc_0_is_open) {
			err = 0; /* TTC 0 device already initialized */
		} else {
			strncpy(ttc_0_config.port_config, "/dev/spidev2.0",
				sizeof(ttc_0_config.port_config));
			ttc_0_config.id = SL_TTC2_RADIO_0;

			ttc_config = ttc_0_config;
		}

		break;
	case TTC_1:
		if (ttc_1_is_open) {
			err = 0; /* TTC 1 device already initialized */
		} else {
			strncpy(ttc_1_config.port_config, "/dev/spidev2.1",
				sizeof(ttc_1_config.port_config));
			ttc_1_config.id = SL_TTC2_RADIO_1;

			ttc_config = ttc_1_config;
		}

		break;
	default:
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, TTC_MODULE_NAME,
			"Error initializing the TTC device! Invalid device!");

		err = -1;

		break;
	}

	if (err == -2) {
		sys_log_print_event_from_module(SYS_LOG_INFO, TTC_MODULE_NAME,
						"Initializing TTC device %u...",
						ttc_config.id);

		if (sl_ttc2_init(&ttc_config) == 0) {
			uint8_t hw_ver = 0;

			if (sl_ttc2_read_hardware_version(&ttc_config,
							  &hw_ver) == 0) {
				uint32_t fw_ver = 0;

				if (sl_ttc2_read_firmware_version(
					    &ttc_config, &fw_ver) == 0) {
					sys_log_print_event_from_module(
						SYS_LOG_INFO, TTC_MODULE_NAME,
						"SpaceLab TTC 2.0 detected! (hw=%u, fw=%u)",
						hw_ver, fw_ver);

					switch (dev) {
					case TTC_0:
						ttc_0_is_open = true;
						break;
					case TTC_1:
						ttc_1_is_open = true;
						break;
					default:
						break;
					}

					err = 0;
				} else {
					sys_log_print_event_from_module(
						SYS_LOG_ERROR, TTC_MODULE_NAME,
						"Error reading the firmware version of the TTC device %u!",
						ttc_config.id);

					err = -1;
				}
			} else {
				sys_log_print_event_from_module(
					SYS_LOG_ERROR, TTC_MODULE_NAME,
					"Error reading the hardware version of the TTC device %u!",
					ttc_config.id);

				err = -1;
			}
		} else {
			sys_log_print_event_from_module(
				SYS_LOG_ERROR, TTC_MODULE_NAME,
				"Error initializing the TTC device %u!",
				ttc_config.id);

			err = -1;
		}
	}

	return err;
}

int ttc_set_param(ttc_e dev, ttc_param_id_t param, uint32_t val)
{
	int err = -1;

	switch (dev) {
	case TTC_0:
		err = sl_ttc2_write_reg(&ttc_0_config, param, val);
		break;
	case TTC_1:
		err = sl_ttc2_write_reg(&ttc_1_config, param, val);
		break;
	default:
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, TTC_MODULE_NAME,
			"Error writing a parameter to the TTC device! Invalid device!");
		break;
	}

	return err;
}

int ttc_get_param(ttc_e dev, ttc_param_id_t param, uint32_t *val)
{
	int err = -1;

	switch (dev) {
	case TTC_0:
		err = sl_ttc2_read_reg(&ttc_0_config, param, val);
		break;
	case TTC_1:
		err = sl_ttc2_read_reg(&ttc_1_config, param, val);
		break;
	default:
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, TTC_MODULE_NAME,
			"Error reading a parameter from the TTC device! Invalid device!");

		break;
	}

	return err;
}

int ttc_get_data(ttc_e dev, ttc_data_t *data)
{
	int err = -1;

	ttc_config_t ttc_config = { 0 };

	switch (dev) {
	case TTC_0:
		ttc_config = ttc_0_config;
		err = 0;
		break;
	case TTC_1:
		ttc_config = ttc_1_config;
		err = 0;
		break;
	default:
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, TTC_MODULE_NAME,
			"Error initializing the TTC device! Invalid device!");

		break;
	}

	if (err == 0) {
		if (sl_ttc2_check_device(&ttc_config) == 0) {
			if (sl_ttc2_read_hk_data(&ttc_config, data) != 0) {
				sys_log_print_event_from_module(
					SYS_LOG_ERROR, TTC_MODULE_NAME,
					"Error reading the data from the TTC device %u!",
					ttc_config.id);

				err = -1;
			}
		} else {
			sys_log_print_event_from_module(
				SYS_LOG_ERROR, TTC_MODULE_NAME,
				"Error reading the data from the TTC device %u!",
				ttc_config.id);
		}
	}

	return err;
}

int ttc_send(ttc_e dev, uint8_t *data, uint16_t len)
{
	int err = -1;

	ttc_config_t ttc_config = { 0 };

	switch (dev) {
	case TTC_0:
		ttc_config = ttc_0_config;
		err = 0;
		break;
	case TTC_1:
		ttc_config = ttc_1_config;
		err = 0;
		break;
	default:
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, TTC_MODULE_NAME,
			"Error initializing the TTC device! Invalid device!");

		break;
	}

	if (err == 0) {
		if (sl_ttc2_check_device(&ttc_config) == 0) {
			if (sl_ttc2_transmit_packet(&ttc_config, data, len) !=
			    0) {
				sys_log_print_event_from_module(
					SYS_LOG_ERROR, TTC_MODULE_NAME,
					"Error sending data to the TTC device %u!",
					ttc_config.id);

				err = -1;
			}
		} else {
			sys_log_print_event_from_module(
				SYS_LOG_ERROR, TTC_MODULE_NAME,
				"Error sending data to the TTC device %u!",
				ttc_config.id);

			err = -1;
		}
	}

	return err;
}

int ttc_recv(ttc_e dev, uint8_t *data, uint16_t *len)
{
	int err = -1;

	ttc_config_t ttc_config = { 0 };

	switch (dev) {
	case TTC_0:
		ttc_config = ttc_0_config;
		err = 0;
		break;
	case TTC_1:
		ttc_config = ttc_1_config;
		err = 0;
		break;
	default:
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, TTC_MODULE_NAME,
			"Error sending data! Invalid device!");

		break;
	}

	if (err == 0) {
		if (ttc_avail(dev) > 0) {
			if (sl_ttc2_read_packet(&ttc_config, data, len) != 0) {
				err = -1;
			}
		} else {
			/* No packet to receive! */
			err = -1;
		}
	}

	return err;
}

int ttc_avail(ttc_e dev)
{
	int err = -1;

	ttc_config_t ttc_config = { 0 };

	switch (dev) {
	case TTC_0:
		ttc_config = ttc_0_config;
		err = 0;
		break;
	case TTC_1:
		ttc_config = ttc_1_config;
		err = 0;
		break;
	default:
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, TTC_MODULE_NAME,
			"Error checking packet availability! Invalid device!");

		break;
	}

	if (err == 0) {
		err = sl_ttc2_check_pkt_avail(&ttc_config);
	}

	return err;
}

int ttc_enter_hibernation(ttc_e dev)
{
	int err = -1;

	ttc_config_t ttc_config = { 0 };

	switch (dev) {
	case TTC_0:
		ttc_config = ttc_0_config;
		err = 0;
		break;
	case TTC_1:
		ttc_config = ttc_1_config;
		err = 0;
		break;
	default:
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, TTC_MODULE_NAME,
			"Error enabling hibernation! Invalid device!");

		break;
	}

	if (err == 0) {
		err = sl_ttc2_set_tx_enable(&ttc_config, false);
	}

	return err;
}

int ttc_leave_hibernation(ttc_e dev)
{
	int err = -1;

	ttc_config_t ttc_config = { 0 };

	switch (dev) {
	case TTC_0:
		ttc_config = ttc_0_config;
		err = 0;
		break;
	case TTC_1:
		ttc_config = ttc_1_config;
		err = 0;
		break;
	default:
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, TTC_MODULE_NAME,
			"Error disabling hibernation! Invalid device!");

		break;
	}

	if (err == 0) {
		err = sl_ttc2_set_tx_enable(&ttc_config, true);
	}

	return err;
}

int ttc_check_failed_pkts(ttc_e dev)
{
	int err = 0;

	uint32_t n_conseq_failed_packets;

	if (ttc_get_param(dev, SL_TTC2_REG_CONSEQ_FAILED_PACKETS,
			  &n_conseq_failed_packets) == 0) {
		if (n_conseq_failed_packets >= TTC_MAX_FAILED_PACKETS) {
			/* Try to reset TTC */
			if (ttc_set_param(dev, SL_TTC2_REG_RESET_DEVICE,
					  0x01U) != 0) {
				sys_log_print_event_from_module(
					SYS_LOG_ERROR, TTC_MODULE_NAME,
					"Failed to reset TTC device after too many failed packets!");
				err = -1;
			}
		}
	} else {
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, TTC_MODULE_NAME,
			"Failed to read number of failed packets from TTC device!");
		err = -1;
	}

	return err;
}

void ttc_print_data(const ttc_e dev, const ttc_data_t *data)
{
	const char *dev_str[] = { "TTC0", "TTC1" };

	const char *const module = dev_str[dev];

	sys_log_print_event_from_module(SYS_LOG_INFO, module,
					"uC Temperature: %u K",
					(uint32_t)data->temperature_mcu);

	sys_log_print_event_from_module(SYS_LOG_INFO, module,
					"Radio Temperature: %u K",
					(uint32_t)data->temperature_radio);

	sl_ttc2_delay_ms(10U);

	sys_log_print_event_from_module(SYS_LOG_INFO, module,
					"uC Current: %u mA",
					(uint32_t)data->current_mcu);

	sys_log_print_event_from_module(SYS_LOG_INFO, module,
					"Radio Current: %u mA",
					(uint32_t)data->current_radio);
	sl_ttc2_delay_ms(10U);

	sys_log_print_event_from_module(SYS_LOG_INFO, module,
					"uC Voltage: %u mV",
					(uint32_t)data->voltage_mcu);

	sys_log_print_event_from_module(SYS_LOG_INFO, module,
					"Radio Voltage: %u mV",
					(uint32_t)data->voltage_radio);

	sys_log_print_event_from_module(SYS_LOG_INFO, module,
					"TX Counter: %u pkts",
					(uint32_t)data->tx_packet_counter);

	sl_ttc2_delay_ms(10U);

	sys_log_print_event_from_module(SYS_LOG_INFO, module,
					"RX Counter: %u pkts",
					(uint32_t)data->rx_packet_counter);
}

/** \} End of ttc group */
