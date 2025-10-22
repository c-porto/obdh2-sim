/*
 * sl_ttc2_mutex.c
 *
 * Copyright The OBDH 2.0 Contributors
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
 * \brief SpaceLab TTC 2.0 driver mutex implementation.
 *
 * \author Carlos Augusto Porto Freitas <carlos.portof@hotmail.com>
 *
 * \version 0.10.14
 *
 * \date 2024/02/26
 *
 * \addtogroup sl_ttc2
 * \{
 */

#include <stddef.h>
#include <pthread.h>

#include <drivers/sl_ttc2.h>

static pthread_mutex_t ttc_mutex = PTHREAD_MUTEX_INITIALIZER;

int sl_ttc2_mutex_take(void)
{
    return pthread_mutex_lock(&ttc_mutex);
}

int sl_ttc2_mutex_give(void)
{
    return pthread_mutex_unlock(&ttc_mutex);
}

/** \} End of sl_ttc2_mutex group */
