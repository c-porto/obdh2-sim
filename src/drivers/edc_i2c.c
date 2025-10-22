/*
 * edc_i2c.c
 * 
 * Copyright (C) 2021, SpaceLab.
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
 * \brief EDC driver I2C routines implementation.
 * 
 * \author Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
 * 
 * \version 0.9.2
 * 
 * \date 2021/10/16
 * 
 * \addtogroup edc
 * \{
 */

#include <drivers/edc.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

int edc_i2c_init(edc_config_t *config)
{
	int err = -1;

	/* Check if the I2C controller path exists */
	if (access(config->i2c_dev, F_OK) != -1) {
		err = 0;
	}

	return err;
}

int edc_i2c_write(edc_config_t *config, uint8_t *data, uint16_t len)
{
	int fd;

	fd = open(config->i2c_dev, O_WRONLY);

	if (fd < 0) {
		perror("Could not open i2c device");
		return -1;
	}

	if (ioctl(fd, I2C_SLAVE, EDC_SLAVE_ADDRESS) < 0) {
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

int edc_i2c_read(edc_config_t *config, uint8_t *data, uint16_t len)
{
	int fd;

	fd = open(config->i2c_dev, O_RDONLY);

	if (fd < 0) {
		perror("Could not open i2c device");
		return -1;
	}

	if (ioctl(fd, I2C_SLAVE, EDC_SLAVE_ADDRESS) < 0) {
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

/** \} End of edc group */
