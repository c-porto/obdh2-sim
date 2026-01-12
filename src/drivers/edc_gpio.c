/*
 * edc_gpio.c
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
 * \brief EDC driver GPIO routines implementation.
 * 
 * \author Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
 * 
 * \version 0.8.15
 * 
 * \date 2021/10/16
 * 
 * \addtogroup edc
 * \{
 */

#include <drivers/edc.h>
#include <stdio.h>

int edc_gpio_init(edc_config_t *config)
{
	config->chip = gpiod_chip_open_by_label("edc-gpio-en");

	if (!config->chip) {
		perror("open gpiod chip edc");
		return -1;
	}

	config->en_line = gpiod_chip_get_line(config->chip, config->en_pin);

	if (!config->en_line) {
		perror("get gpiod line edc");
		gpiod_chip_close(config->chip);
		return -1;
	}

	/* Request line as output, default LOW */
	int ret = gpiod_line_request_output(config->en_line, "edc-enable", 0);
	if (ret < 0) {
		perror("request gpiod line output edc");
		gpiod_chip_close(config->chip);
		return -1;
	}

	return 0;
}

int edc_gpio_set(edc_config_t *config)
{
	if (!config->chip || !config->en_line)
		return -1;

	gpiod_line_set_value(config->en_line, 1);

	return 0;
}

int edc_gpio_clear(edc_config_t *config)
{
	if (!config->chip || !config->en_line)
		return -1;

	gpiod_line_set_value(config->en_line, 0);

	return 0;
}

/** \} End of edc group */
