/*
 * edc_uart.c
 * 
 * Copyright The EDC Test App Contributors.
 * 
 * This file is part of EDC-Test-App.
 * 
 * EDC-Test-App is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * EDC-Test-App is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with EDC-Tets-App. If not, see <http://www.gnu.org/licenses/>.
 * 
 */

/**
 * \brief EDC driver UART routines implementation.
 *
 * \author Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
 *
 * \version 0.0.7
 *
 * \date 2022/03/15
 *
 * \addtogroup edc
 * \{
 */

#include <stdio.h>
#include <unistd.h>

#include <system/sys_log.h>
#include <drivers/uart.h>
#include <drivers/edc.h>

static int fd = -1;

int edc_uart_init(edc_config_t *config)
{
	int err = 0;

	fd = uart_open(config->uart_port, 115200, UART_PARITY_NONE,
		       UART_STOP_BITS_ONE, UART_DATA_BITS_8);

	if (fd < 0) {
		err = -1;

		sys_log_print_event_from_module(
			SYS_LOG_ERROR, "edc_uart",
			"Error opening the UART port \"%s\" @ 115200 bps!",
			config->uart_port);
	}

	return err;
}

int edc_uart_write(edc_config_t *config, uint8_t *data, uint16_t len)
{
	int err = -1;

	if (!uart_is_open(fd)) {
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, "edc_uart",
			"Error writing data to the UART port! UART port not opened!\n\r");
	} else {
		err = uart_write(fd, data, len);
	}

	return err;
}

int edc_uart_read(edc_config_t *config, uint8_t *data, uint16_t len)
{
	int err = -1;

	if (!uart_is_open(fd)) {
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, "edc_uart",
			"Error reading data from the UART port! UART port not opened!\n\r");
	} else {
		uint32_t period_ms = 10;
		uint32_t timeout_ms = 100;

		uint16_t avail = 0U;

		uint16_t i = 0U;
		for (i = 0U; i < timeout_ms / period_ms; i++) {
			avail = uart_available(fd);

			if (avail >= len) {
				if (uart_read(fd, data, avail) !=
				    UART_SUCCESS) {
					sys_log_print_event_from_module(
						SYS_LOG_ERROR, "edc_uart",
						"Error reading a response!\n\r");
				} else {
					err = 0;
				}

				break;
			}

			usleep(period_ms * 1000); /* Milliseconds delay */
		}

		if (i >= (timeout_ms / period_ms)) {
			printf("Timeout reached waiting for a response!\n\r");
		}
	}

	return err; /* Timeout reached! */
}

int edc_uart_rx_available(edc_config_t *config)
{
	return (int)uart_available(fd);
}

/** \} End of edc group */
