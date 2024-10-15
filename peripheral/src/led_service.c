/*
 * Copyright (c) 2024 inovex GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/types.h>
#include <zephyr/kernel.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(led_service);

#include "demo/bluetooth/services/led_service.h"

static led_set_enabled_func set_enabled;
static int current_state;

static ssize_t read_led(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf,
			uint16_t len, uint16_t offset)
{
	(void)conn;
	(void)attr;
	uint8_t state_byte = (uint8_t)current_state;
	LOG_INF("LED state read, returning 0x%02" PRIx8, state_byte);
	return bt_gatt_attr_read(conn, attr, buf, len, offset, &state_byte, sizeof(state_byte));
}

static ssize_t write_led(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf,
			 uint16_t len, uint16_t offset, uint8_t flags)
{
	(void)conn;
	(void)attr;
	if (len != 1u) {
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}
	if (offset != 0u) {
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}
	uint8_t new_state = ((const uint8_t *)buf)[0];
	if ((new_state != 0x00) && (new_state != 0x01)) {
		return BT_GATT_ERR(BT_ATT_ERR_VALUE_NOT_ALLOWED);
	}
	LOG_INF("LED state write 0x%02" PRIx8, new_state);
	if (new_state != current_state) {
		if (set_enabled) {
			set_enabled(new_state);
		}
		current_state = new_state;
	}
	return len;
}

BT_GATT_SERVICE_DEFINE(led_service, BT_GATT_PRIMARY_SERVICE(BT_UUID_LED_SERVICE),
		       BT_GATT_CHARACTERISTIC(BT_UUID_LED_CHARACTERISTIC,
					      BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
					      BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_led,
					      write_led, NULL), );

void led_service_init(led_set_enabled_func cb)
{
	set_enabled = cb;
}
