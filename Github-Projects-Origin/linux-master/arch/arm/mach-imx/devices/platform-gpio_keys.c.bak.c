#include <assert.h>
#include <string.h>
#define INCLUDEMAIN
/*
 * Copyright (C) 2010 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */
#include <asm/sizes.h>

#include "../hardware.h"
#include "devices-common.h"

struct platform_device *__init imx_add_gpio_keys(
		const struct gpio_keys_platform_data *pdata)
{
	return imx_add_platform_device("gpio-keys", -1, NULL,
		 0, pdata, sizeof(*pdata));
}