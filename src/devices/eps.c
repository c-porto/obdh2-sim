/*
 * eps.c
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
 * \brief EPS device implementation.
 * 
 * \author Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
 * 
 * \version 0.8.35
 * 
 * \date 2020/02/01
 * 
 * \addtogroup eps
 * \{
 */

#include <stdbool.h>

#include <drivers/sl_eps2.h>

#include <devices/eps.h>

static sl_eps2_config_t eps_config = {0};

static bool eps_is_open = false;

int eps_init(void)
{
    int err = -1;

    if (eps_is_open)
    {
        err = 0;    /* EPS device already initialized */
    }
    else
    {
        int err_drv = sl_eps2_init(eps_config);

        if (err_drv == 0)
        {
            eps_is_open = true;

            err = 0;
        }
    }

    return err;
}

int eps_set_param(eps_param_id_t param, uint32_t val)
{
    return sl_eps2_write_reg(eps_config, param, val);
}

int eps_get_param(eps_param_id_t param, uint32_t *val)
{
    return sl_eps2_read_reg(eps_config, param, val);
}

int eps_get_bat_voltage(eps_voltage_t *bat_volt)
{
    int err = -1;

    if (eps_is_open)
    {
        int err_drv = sl_eps2_read_battery_voltage(eps_config, bat_volt);

        if (err_drv == 0)
        {
            err = 0;
        }
    }

    return err;
}

int eps_get_bat_current(eps_current_t *bat_cur)
{
    int err = -1;

    if (eps_is_open)
    {
        int err_drv = sl_eps2_read_battery_current(eps_config, SL_EPS2_BATTERY_CURRENT, bat_cur);

        if (err_drv == 0)
        {
            err = 0;
        }
    }

    return err;
}

int eps_get_bat_charge(eps_charge_t *charge)
{
    int err = -1;

    if (eps_is_open)
    {
        int err_drv = sl_eps2_read_battery_charge(eps_config, charge);

        if (err_drv == 0)
        {
            err = 0;
        }
    }

    return err;
}

int eps_get_data(eps_data_t *data)
{
    int err = -1;

    if (eps_is_open)
    {
        int err_drv = sl_eps2_read_data(eps_config, data);

        if (err_drv == 0)
        {
            err = 0;
        }
    }

    return err;
}

/** \} End of eps group */
