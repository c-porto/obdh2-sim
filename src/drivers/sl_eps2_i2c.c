/*
 * sl_eps2_i2c.c
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
 * \brief SpaceLab EPS 2.0 driver I2C implementation.
 *
 * \author Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
 *
 * \version 0.8.41
 *
 * \date 2021/10/13
 *
 * \addtogroup sl_eps2
 * \{
 */

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <drivers/sl_eps2.h>

#define I2C_CTRL_PATH "/dev/i2c-2"

int sl_eps2_i2c_init(sl_eps2_config_t config)
{
	int err = -1;

	/* Check if the I2C controller path exists */
	if (access(I2C_CTRL_PATH, F_OK) != -1) {
		err = 0;
	}

	return err;
}

int sl_eps2_i2c_write(sl_eps2_config_t config, uint8_t *data, uint16_t len)
{
	int fd;

	fd = open(I2C_CTRL_PATH, O_WRONLY);

	if (fd < 0) {
		perror("Could not open i2c device");
		return -1;
	}

	if (ioctl(fd, I2C_SLAVE, SL_EPS2_I2C_SLAVE_ADR) < 0) {
		perror("Failed to acquire bus access and/or talk to slave");
		close(fd);
		return -1;
	}

	if (write(fd, data, len) != len) {
		perror("Could not write to i2c device");
		close(fd);
		return -1;
	}

	close(fd);
	return 0;
}

int sl_eps2_i2c_read(sl_eps2_config_t config, uint8_t *data, uint16_t len)
{
	int fd;

	fd = open(I2C_CTRL_PATH, O_RDONLY);

	if (fd < 0) {
		perror("Could not open i2c device");
		return -1;
	}

	if (ioctl(fd, I2C_SLAVE, SL_EPS2_I2C_SLAVE_ADR) < 0) {
		perror("Failed to acquire bus access and/or talk to slave");
		close(fd);
		return -1;
	}

	if (read(fd, data, len) != len) {
		perror("Could not read to i2c device");
		close(fd);
		return -1;
	}

	close(fd);
	return 0;
}

/** \} End of sl_eps2 group */
