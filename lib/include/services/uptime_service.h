/*
 * Copyright (c) 2024 inovex GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef DEMO_SERVICES_UPTIME_SERVICE_H_
#define DEMO_SERVICES_UPTIME_SERVICE_H_

#define BT_UUID_UPTIME_SERVICE_VAL                                                                 \
	BT_UUID_128_ENCODE(0x9f2cfd1e, 0x3400, 0x461e, 0x98d6, 0x1fa955fd80a4)
#define BT_UUID_UPTIME_SERVICE BT_UUID_DECLARE_128(BT_UUID_UPTIME_SERVICE_VAL)

#define BT_UUID_UPTIME_CHARACTERISTIC_VAL                                                          \
	BT_UUID_128_ENCODE(0x9f2cfd1e, 0x3401, 0x461e, 0x98d6, 0x1fa955fd80a4)
#define BT_UUID_UPTIME_CHARACTERISTIC BT_UUID_DECLARE_128(BT_UUID_UPTIME_CHARACTERISTIC_VAL)

#endif /* DEMO_SERVICES_UPTIME_SERVICE_H_ */
