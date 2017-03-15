cmake_minimum_required(VERSION 3.7)
project(hermit_include)

include(../../cmake/HermitCore.cmake)

add_custom_command(
	OUTPUT
		${CMAKE_CURRENT_LIST_DIR}/config.h
	DEPENDS
		${CMAKE_CURRENT_LIST_DIR}/config.h.in
		${HERMIT_ROOT}/configure
	WORKING_DIRECTORY
		${HERMIT_ROOT}
	USES_TERMINAL
	COMMAND
		./configure
)
add_custom_target(config-h DEPENDS ${CMAKE_CURRENT_LIST_DIR}/config.h)

add_custom_command(
	OUTPUT
		${CMAKE_CURRENT_LIST_DIR}/config.inc
	DEPENDS
		${CMAKE_CURRENT_LIST_DIR}/config.h
	WORKING_DIRECTORY
		${CMAKE_CURRENT_LIST_DIR}
	USES_TERMINAL
	COMMAND
		echo '\; This file is generated automatically from the config.h file.' > config.inc
	COMMAND
		echo '\; Before editing this, you should consider editing config.h.' >> config.inc
	COMMAND
		awk '/^\#define MAX_CORES/{ print \"%define MAX_CORES\", $$3 }' config.h >> config.inc
	COMMAND
		awk '/^\#define KERNEL_STACK_SIZE/{ print \"%define KERNEL_STACK_SIZE\", $$3 }' config.h >> config.inc
	COMMAND
		awk '/^\#define VIDEO_MEM_ADDR/{ print \"%define VIDEO_MEM_ADDR\", $$3 }' config.h >> config.inc
	COMMAND
		awk '/^\#define CONFIG_VGA/{ print \"%define CONFIG_VGA\", $$3 }' config.h >> config.inc
	COMMAND
		awk '/^\#define DYNAMIC_TICKS/{ print \"%define DYNAMIC_TICKS\", $$3 }' config.h >> config.inc
	COMMAND
		awk '/^\#define SAVE_FPU/{ print \"%define SAVE_FPU\", $$3 }' config.h >> config.inc
	)

add_custom_target(config-inc DEPENDS ${CMAKE_CURRENT_LIST_DIR}/config.inc)
