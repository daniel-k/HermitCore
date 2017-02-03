cmake_minimum_required(VERSION 3.7)
project(hermit_tools)

include(../cmake/HermitCore-Paths.cmake)

add_executable(proxy proxy.c uhyve.c)
target_link_libraries(proxy pthread)

install(TARGETS proxy
		DESTINATION ${HERMIT_BIN_DIR})
