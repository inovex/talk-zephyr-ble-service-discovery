# Copyright (c) 2024 inovex GmbH
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(peripheral)

target_sources(app PRIVATE
	src/main.c
	src/led_service.c
	src/uptime_service.c
)
target_include_directories(app PRIVATE
	"${CMAKE_CURRENT_LIST_DIR}/../include"
)
