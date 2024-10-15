/*
 * Copyright (c) 2024 inovex GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef DEMO_SERVICE_DISCOVERY_H_
#define DEMO_SERVICE_DISCOVERY_H_

#include <zephyr/bluetooth/bluetooth.h>

struct service_handles {
	uint16_t led_value;
	uint16_t uptime_value;
};

typedef void (*service_discovery_cb)(struct bt_conn *conn, int success, struct service_handles *handles);

void discover_services(struct bt_conn *conn, service_discovery_cb cb);

#endif /* DEMO_SERVICE_DISCOVERY_H_ */
