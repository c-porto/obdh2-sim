/*
 * sl_ttc2_spi.c
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
 * \brief SpaceLab TTC 2.0 driver SPI implementation.
 * 
 * \author Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
 * \author Carlos Augusto Porto Freitas <carlos.portof@hotmail.com>
 * 
 * \version 1.0.0
 * 
 * \date 2021/10/13
 * 
 * \addtogroup sl_ttc2
 * \{
 */

#include <drivers/sl_ttc2.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define CRC8_INITIAL_VAL 0x00U /* CRC8-CCITT initial value. */
#define CRC8_POLYNOMIAL 0x07U /* CRC8-CCITT polynomial. */

static uint8_t crc8_get_val(uint8_t *data, uint8_t len)
{
	uint8_t crc = CRC8_INITIAL_VAL;
	uint8_t i = 0U;
	uint8_t j = 0U;

	for (i = 0U; i < len; i++) {
		crc ^= data[i];

		j = 0U;
		for (j = 0U; j < 8U; j++) {
			crc = (crc << 1) ^
			      ((crc & 0x80U) ? CRC8_POLYNOMIAL : 0U);
		}

		crc &= 0xFFU;
	}

	return crc;
}

int sl_ttc2_spi_init(sl_ttc2_config_t *config)
{
	return 0;
}

int sl_ttc2_spi_write(sl_ttc2_config_t *config, uint8_t *data, uint16_t len)
{
	uint8_t rbuf[230] = { 0 };

	return sl_ttc2_spi_transfer(config, data, rbuf, len);
}

int sl_ttc2_spi_transfer(sl_ttc2_config_t *config, uint8_t *wdata,
			 uint8_t *rdata, uint16_t len)
{
	int ret = 0;

	int fd = open(config->port_config, O_RDWR);

	if (fd < 0) {
		perror("sl_ttc2: open SPI devices");
		return -1;
	}

	int mode = SPI_MODE_0;

	ioctl(fd, SPI_IOC_WR_MODE, &mode);

	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)wdata,
		.rx_buf = (unsigned long)rdata,
		.len = len,
		.delay_usecs = 0,
		.speed_hz = 1000000,
		.bits_per_word = 8,
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);

	if (ret < 0) {
		perror("sl_ttc: SPI tranfer!");
	}

	close(fd);

	return ret;
}

int sl_ttc2_spi_read(sl_ttc2_config_t *config, uint8_t *data, uint16_t len)
{
	uint8_t wbuf[230] = { 0 };

	/* Adding preamble byte */
	wbuf[0] = 0x7EU;

	/* Adding CRC */
	wbuf[len - 1U] = crc8_get_val(wbuf, len - 1U);

	return sl_ttc2_spi_transfer(config, wbuf, data, len);
}

/** \} End of sl_ttc2 group */
