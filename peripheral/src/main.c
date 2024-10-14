/*
 * Copyright (c) 2024 inovex GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>

#include <zephyr/drivers/gpio.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(peripheral_main);

#include "demo/bluetooth/services/led_service.h"

enum app_event {
	APP_EVENT_CONNECTED = BIT(0),
	APP_EVENT_DISCONNECTED = BIT(1),
};
static K_EVENT_DEFINE(app_event);

#define SERVICE_LED_NODE DT_ALIAS(service_led)
static const struct gpio_dt_spec service_led = GPIO_DT_SPEC_GET(SERVICE_LED_NODE, gpios);

static void control_led(int enabled)
{
	int err = gpio_pin_set_dt(&service_led, enabled);
	if (err) {
		LOG_ERR("failed to set service led to %d: %d", enabled, err);
	}
}

static void on_connected(struct bt_conn *conn, uint8_t err)
{
	LOG_INF("device connected: %d", err);
	k_event_post(&app_event, APP_EVENT_CONNECTED);
}

static void on_disconnected(struct bt_conn *conn, uint8_t err)
{
	LOG_INF("device disconnected: %d", err);
	k_event_post(&app_event, APP_EVENT_DISCONNECTED);
}

static struct bt_conn_cb connection_callbacks = {
	.connected = on_connected,
	.disconnected = on_disconnected,
};

static struct bt_data advertisement_data[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_LIMITED | BT_LE_AD_NO_BREDR),
	BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};
static const struct bt_le_adv_param *advertisement_params = BT_LE_ADV_PARAM(
	BT_LE_ADV_OPT_CONNECTABLE, 32 /* 20 ms */, 48 /* 30 ms */, NULL /* undirected */);

static void print_ble_addrs(void)
{
	size_t count;
	char buf[BT_ADDR_LE_STR_LEN];
	bt_addr_le_t addrs[CONFIG_BT_ID_MAX];

	bt_id_get(&addrs[0], &count);
	if (count == 0) {
		LOG_INF("no BLE addresses available");
		return;
	}
	LOG_INF("BLE addresses:");
	for (size_t i = 0; i < count; i++) {
		bt_addr_le_to_str(&addrs[i], &buf[0], BT_ADDR_LE_STR_LEN);
		LOG_INF("\t%s", buf);
	}
}

static int start_advertising()
{
	LOG_INF("start advertising");
	int err = bt_le_adv_start(advertisement_params, advertisement_data,
				  ARRAY_SIZE(advertisement_data), NULL, 0);
	if (err) {
		LOG_ERR("failed to stop Bluetooth advertising: %d", err);
	}
	return err;
}

static int stop_advertising()
{
	LOG_INF("stop advertising");
	int err = bt_le_adv_stop();
	if (err) {
		LOG_ERR("failed to stop Bluetooth advertising: %d", err);
	}
	return err;
}

int main(void)
{
	if (!gpio_is_ready_dt(&service_led)) {
		LOG_ERR("service led not ready");
		return 1;
	}
	int err = gpio_pin_configure_dt(&service_led, GPIO_OUTPUT_INACTIVE);
	if (err) {
		LOG_ERR("failed to configure led: %d", err);
		return 1;
	}
	led_service_init(control_led);
	bt_conn_cb_register(&connection_callbacks);
	bt_enable(NULL);
	if (start_advertising()) {
		return 1;
	}
	print_ble_addrs();
	uint32_t events;
	for (;;) {
		events = k_event_wait(&app_event, APP_EVENT_CONNECTED | APP_EVENT_DISCONNECTED,
				      true, K_FOREVER);
		if (events & APP_EVENT_CONNECTED) {
			if (stop_advertising()) {
				return 1;
			}

		} else if (events & APP_EVENT_DISCONNECTED) {
			if (start_advertising()) {
				return 1;
			}
		}
	}
	return 0;
}
