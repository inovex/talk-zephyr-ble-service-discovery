/*
 * Copyright (c) 2024 inovex GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef DEMO_SERVICES_LED_SERVICE_H_
#define DEMO_SERVICES_LED_SERVICE_H_

#define BT_UUID_LED_SERVICE_VAL                                                                 \
	BT_UUID_128_ENCODE(0xbf3856b1, 0xa600, 0x4d6e, 0x86e3, 0x060f20cc7680)
#define BT_UUID_LED_SERVICE BT_UUID_DECLARE_128(BT_UUID_LED_SERVICE_VAL)

#define BT_UUID_LED_CHARACTERISTIC_VAL                                                          \
	BT_UUID_128_ENCODE(0xbf3856b1, 0xa601, 0x4d6e, 0x86e3, 0x060f20cc7680)
#define BT_UUID_LED_CHARACTERISTIC BT_UUID_DECLARE_128(BT_UUID_LED_CHARACTERISTIC_VAL)

typedef void (*led_set_enabled_func)(int enabled);

void led_service_init(led_set_enabled_func cb);

#endif /* DEMO_SERVICES_LED_SERVICE_H_ */
