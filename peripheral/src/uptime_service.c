/*
 * Copyright (c) 2024 inovex GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/types.h>
#include <zephyr/kernel.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>

#include "services/uptime_service.h"

static ssize_t read_uptime(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf,
			   uint16_t len, uint16_t offset)
{
	(void)conn;
	(void)attr;
	int64_t uptime = k_uptime_get();
	return bt_gatt_attr_read(conn, attr, buf, len, offset, &uptime, sizeof(uptime));
}

BT_GATT_SERVICE_DEFINE(uptime_service, BT_GATT_PRIMARY_SERVICE(BT_UUID_UPTIME_SERVICE),
		       BT_GATT_CHARACTERISTIC(BT_UUID_UPTIME_CHARACTERISTIC, BT_GATT_CHRC_READ,
					      BT_GATT_PERM_READ, read_uptime, NULL, NULL), );
