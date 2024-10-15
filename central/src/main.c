/*
 * Copyright (c) 2024 inovex GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(central_main);

#include "service_discovery.h"
#include "use_services.h"

/* For now, the hard-coded peripheral address E3:27:C3:70:1D:F6 is used */
static const bt_addr_le_t peripheral_addr = {
	.type = BT_ADDR_LE_RANDOM,
	.a = {.val = {0xF6, 0x1D, 0x70, 0xC3, 0x27, 0xE3}},
};

static int start_scanning(void);

static void on_services_discovered(struct bt_conn *conn, int success,
				   struct service_handles *handles)
{
	if (success) {
		LOG_INF("services discovered");
		use_services(conn, handles);
	} else {
		LOG_ERR("service discovery failed");
	}
}

static void on_connected(struct bt_conn *conn, uint8_t err)
{
	LOG_INF("device connected: %d", err);
	if (err) {
		LOG_ERR("failed to connect to device: %d", err);
		start_scanning();
		return;
	}
	discover_services(conn, on_services_discovered);
}

static void on_disconnected(struct bt_conn *conn, uint8_t err)
{
	LOG_INF("device disconnected: %d", err);
	start_scanning();
}

BT_CONN_CB_DEFINE(connection_callbacks) = {
	.connected = on_connected,
	.disconnected = on_disconnected,
};

static int connect(const bt_addr_le_t *addr)
{
	LOG_INF("connecting to peripheral...");
	struct bt_conn *conn;
	int err = bt_conn_le_create(addr, BT_CONN_LE_CREATE_CONN, BT_LE_CONN_PARAM_DEFAULT, &conn);
	if (err) {
		char addrstr[BT_ADDR_LE_STR_LEN];
		bt_addr_le_to_str(addr, addrstr, sizeof(addrstr));
		LOG_ERR("failed to connect to %s: %d", addrstr, err);
	}
	return err;
}

static void on_device_scanned(const bt_addr_le_t *addr, int8_t rssi, uint8_t type,
			      struct net_buf_simple *ad)
{
	/* ignore non-connectable advertisements */
	if ((type != BT_GAP_ADV_TYPE_ADV_IND) && (type != BT_GAP_ADV_TYPE_ADV_DIRECT_IND)) {
		return;
	}
	if (!bt_addr_le_eq(addr, &peripheral_addr)) {
		return;
	}
	LOG_INF("peripheral device found");
	int err = bt_le_scan_stop();
	if (err) {
		LOG_ERR("failed to stop scanning");
		return;
	}
	if (connect(addr)) {
		start_scanning();
		return;
	}
}

static int start_scanning(void)
{
	LOG_INF("start scanning...");
	int err = bt_le_scan_start(BT_LE_SCAN_PASSIVE, on_device_scanned);
	if (err) {
		LOG_ERR("failed to start scanning: %d", err);
	}
	return err;
}

int main(void)
{
	int err = bt_enable(NULL);
	if (err) {
		LOG_ERR("failed to initialize Bluetooth: %d", err);
		return 1;
	}
	if (start_scanning()) {
		return 1;
	}
	k_sleep(K_FOREVER);
	return 0;
}
