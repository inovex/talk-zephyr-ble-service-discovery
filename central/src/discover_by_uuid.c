/*
 * Copyright (c) 2024 inovex GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef CONFIG_DEMO_GATT_SD_BY_UUID

#include "service_discovery.h"

#include <zephyr/bluetooth/gatt.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(service_discovery);

#include "demo/bluetooth/services/led_service.h"
#include "demo/bluetooth/services/uptime_service.h"

static struct bt_uuid_128 discover_uuid = BT_UUID_INIT_128(0);
static struct bt_gatt_discover_params discover_params;
static struct service_handles discovered_handles;
static service_discovery_cb discovery_cb;

static uint8_t discover_func(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			     struct bt_gatt_discover_params *params)
{
	if (!attr) {
		LOG_INF("discovery completed");
		return BT_GATT_ITER_STOP;
	}
	LOG_INF("discovering attribute handle %u", attr->handle);
	if (!bt_uuid_cmp(&discover_uuid.uuid, BT_UUID_LED_SERVICE)) {
		LOG_INF("discovered LED service");
		memcpy(&discover_uuid, BT_UUID_LED_CHARACTERISTIC, sizeof(discover_uuid));
		discover_params.uuid = &discover_uuid.uuid;
		discover_params.start_handle = attr->handle + 1;
		discover_params.type = BT_GATT_DISCOVER_CHARACTERISTIC;
		int err = bt_gatt_discover(conn, &discover_params);
		if (err) {
			LOG_ERR("failed to discover LED characteristic: %d", err);
			if (discovery_cb) {
				discovery_cb(conn, 0, NULL);
			}
		}
	} else if (!bt_uuid_cmp(&discover_uuid.uuid, BT_UUID_LED_CHARACTERISTIC)) {
		LOG_INF("discovered LED characteristic");
		discovered_handles.led_value = bt_gatt_attr_value_handle(attr);
		memcpy(&discover_uuid, BT_UUID_UPTIME_SERVICE, sizeof(discover_uuid));
		discover_params.uuid = &discover_uuid.uuid;
		discover_params.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE;
		discover_params.type = BT_GATT_DISCOVER_PRIMARY;
		int err = bt_gatt_discover(conn, &discover_params);
		if (err) {
			LOG_ERR("failed to discover LED characteristic: %d", err);
			if (discovery_cb) {
				discovery_cb(conn, 0, NULL);
			}
		}
	} else if (!bt_uuid_cmp(&discover_uuid.uuid, BT_UUID_UPTIME_SERVICE)) {
		LOG_INF("discovered uptime service");
		memcpy(&discover_uuid, BT_UUID_UPTIME_CHARACTERISTIC, sizeof(discover_uuid));
		discover_params.uuid = &discover_uuid.uuid;
		discover_params.start_handle = attr->handle + 1;
		discover_params.type = BT_GATT_DISCOVER_CHARACTERISTIC;
		int err = bt_gatt_discover(conn, &discover_params);
		if (err) {
			LOG_ERR("failed to discover LED characteristic: %d", err);
			if (discovery_cb) {
				discovery_cb(conn, 0, NULL);
			}
		}
	} else if (!bt_uuid_cmp(&discover_uuid.uuid, BT_UUID_UPTIME_CHARACTERISTIC)) {
		LOG_INF("discovered uptime characteristic");
		discovered_handles.uptime_value = bt_gatt_attr_value_handle(attr);
		if (discovery_cb) {
			discovery_cb(conn, 1, &discovered_handles);
		}
	}
	LOG_INF("discovery of handle %u finished", attr->handle);
	return BT_GATT_ITER_STOP;
}

void discover_services(struct bt_conn *conn, service_discovery_cb cb)
{
	discovery_cb = cb;

	memcpy(&discover_uuid, BT_UUID_LED_SERVICE, sizeof(discover_uuid));
	discover_params.uuid = &discover_uuid.uuid;
	discover_params.func = discover_func;
	discover_params.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE;
	discover_params.end_handle = BT_ATT_LAST_ATTRIBUTE_HANDLE;
	discover_params.type = BT_GATT_DISCOVER_PRIMARY;

	LOG_INF("starting service discovery...");
	int err = bt_gatt_discover(conn, &discover_params);
	if (err) {
		LOG_ERR("failed to discover services: %d", err);
	}
	LOG_INF("service discovery done");
}

#endif /* CONFIG_DEMO_GATT_SD_BY_UUID */
