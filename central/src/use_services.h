/*
 * Copyright (c) 2024 inovex GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef DEMO_USE_SERVICES_H_
#define DEMO_USE_SERVICES_H_

#include <zephyr/bluetooth/bluetooth.h>

#include "service_discovery.h"

void use_services(struct bt_conn *conn, struct service_handles *handles);

#endif /* DEMO_USE_SERVICES_H_ */
