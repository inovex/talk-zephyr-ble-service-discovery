/*
 * Copyright (c) 2024 inovex GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef CONFIG_DEMO_GATT_SD_ITER_ALL

#include "service_discovery.h"

#include <zephyr/bluetooth/gatt.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(service_discovery);

#include "demo/bluetooth/services/led_service.h"
#include "demo/bluetooth/services/uptime_service.h"

enum {
	LED_HANDLE_DISCOVERED = BIT(0),
	UPTIME_HANDLE_DISCOVERED = BIT(1),
};

static struct bt_uuid_128 discover_uuid = BT_UUID_INIT_128(0);
static struct bt_gatt_discover_params discover_params;
static struct service_handles discovered_handles;
static service_discovery_cb discovery_cb;
static uint8_t discovered_record;

const char *uuid_as_str(const struct bt_uuid *uuid)
{
	static char buf[BT_UUID_STR_LEN];
	bt_uuid_to_str(uuid, &buf[0], ARRAY_SIZE(buf));
	return &buf[0];
}

static uint8_t discover_func(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			     struct bt_gatt_discover_params *params)
{
	if (!attr) {
		LOG_INF("discovery completed");
		if (discovery_cb) {
			if (discovered_record &
			    (LED_HANDLE_DISCOVERED | UPTIME_HANDLE_DISCOVERED)) {
				discovery_cb(conn, 1, &discovered_handles);
			} else {
				discovery_cb(conn, 0, NULL);
			}
		}
		return BT_GATT_ITER_STOP;
	}
	const char *discover_type = params->type == BT_GATT_DISCOVER_PRIMARY ? "SVC" : "CHRC";
	LOG_INF("discovering %s attribute handle %u", discover_type, attr->handle);
	uint8_t ret = BT_GATT_ITER_CONTINUE;
	if (params->type == BT_GATT_DISCOVER_PRIMARY) {
		struct bt_gatt_service_val *svc = (struct bt_gatt_service_val *)attr->user_data;
		if (!bt_uuid_cmp(svc->uuid, BT_UUID_LED_SERVICE)) {
			LOG_INF("discovered LED service");
			memcpy(&discover_uuid, BT_UUID_LED_CHARACTERISTIC, sizeof(discover_uuid));
			ret = BT_GATT_ITER_STOP;
		} else if (!bt_uuid_cmp(svc->uuid, BT_UUID_UPTIME_SERVICE)) {
			LOG_INF("discovered uptime service");
			memcpy(&discover_uuid, BT_UUID_UPTIME_CHARACTERISTIC,
			       sizeof(discover_uuid));
			ret = BT_GATT_ITER_STOP;
		} else {
			const char *uuid = uuid_as_str(svc->uuid);
			LOG_INF("ignoring service %s", uuid);
		}
		if (ret == BT_GATT_ITER_STOP) {
			discover_params.uuid = &discover_uuid.uuid;
			discover_params.start_handle = attr->handle + 1;
			discover_params.end_handle = svc->end_handle;
			discover_params.type = BT_GATT_DISCOVER_CHARACTERISTIC;
		}
	} else if (params->type == BT_GATT_DISCOVER_CHARACTERISTIC) {
		struct bt_gatt_chrc *chrc = (struct bt_gatt_chrc *)attr->user_data;
		if (!bt_uuid_cmp(chrc->uuid, BT_UUID_LED_CHARACTERISTIC)) {
			LOG_INF("discovered LED characteristic");
			discovered_handles.led_value = bt_gatt_attr_value_handle(attr);
			discovered_record |= LED_HANDLE_DISCOVERED;
			ret = BT_GATT_ITER_STOP;
		} else if (!bt_uuid_cmp(chrc->uuid, BT_UUID_UPTIME_CHARACTERISTIC)) {
			LOG_INF("discovered uptime characteristic");
			discovered_handles.uptime_value = bt_gatt_attr_value_handle(attr);
			discovered_record |= UPTIME_HANDLE_DISCOVERED;
			ret = BT_GATT_ITER_STOP;
		} else {
			const char *uuid = uuid_as_str(chrc->uuid);
			LOG_INF("ignoring characteristic %s", uuid);
		}
		if (ret == BT_GATT_ITER_STOP) {
			discover_params.uuid = NULL;
			discover_params.start_handle = discover_params.end_handle + 1;
			discover_params.end_handle = BT_ATT_LAST_ATTRIBUTE_HANDLE;
			discover_params.type = BT_GATT_DISCOVER_PRIMARY;
		}
	}

	if (ret == BT_GATT_ITER_STOP) {
		int err = bt_gatt_discover(conn, &discover_params);
		if (err) {
			LOG_ERR("failed to discover LED characteristic: %d", err);
			if (discovery_cb) {
				discovery_cb(conn, 0, NULL);
			}
		}
	}
	LOG_INF("discovery of handle %u finished", attr->handle);
	return ret;
}

void discover_services(struct bt_conn *conn, service_discovery_cb cb)
{
	discovery_cb = cb;

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

#endif /* CONFIG_DEMO_GATT_SD_ITER_ALL */
