# Copyright (c) 2024 inovex GmbH
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(central)

target_sources(app PRIVATE
	src/main.c
  src/discover_by_uuid.c
  src/discover_all_services.c
  src/use_services.c
)
target_include_directories(app PRIVATE
	"${CMAKE_CURRENT_LIST_DIR}/../include"
)
