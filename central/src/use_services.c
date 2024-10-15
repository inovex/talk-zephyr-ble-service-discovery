/*
 * Copyright (c) 2024 inovex GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "use_services.h"

#include <zephyr/bluetooth/gatt.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(service_user);

static struct bt_gatt_read_params read_params;

static uint8_t on_uptime_read(struct bt_conn *conn, uint8_t err, struct bt_gatt_read_params *params,
			      const void *data, uint16_t length)
{
	if (err) {
		LOG_ERR("read failed: ATT error: %u", err);
		goto out;
	}
	if (length != sizeof(uint64_t)) {
		LOG_ERR("read failed: received invalid data length");
		goto out;
	}
	uint64_t uptime = 0;
	memcpy(&uptime, data, length);
	LOG_INF("uptime: %" PRIu64, uptime);
out:
	return BT_GATT_ITER_STOP;
}

void use_services(struct bt_conn *conn, struct service_handles *handles)
{
	read_params.func = on_uptime_read;
	read_params.handle_count = 1;
	read_params.single.handle = handles->uptime_value;
	read_params.single.offset = 0;
	bt_gatt_read(conn, &read_params);
}
