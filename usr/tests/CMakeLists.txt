cmake_minimum_required(VERSION 3.7)
include(../../cmake/HermitCore.cmake)

project(hermit_tests C CXX Fortran Go)

add_compile_options(${HERMIT_APP_FLAGS})

# link against local libhermit.a instead of the one supplied with the
# toolchain, if built from top-level
link_directories(${TEST_TARGET} ${CMAKE_BINARY_DIR})

add_executable(hello hello.c)
add_executable(jacobi jacobi.c)
add_executable(hello++ hello++.cpp)
add_executable(hellof hellof.f90)
add_executable(pi pi.go)

add_executable(server server.go)
target_link_libraries(server netgo)

add_executable(RCCE_minimum RCCE_minimum.c)
target_link_libraries(RCCE_minimum ircce)

add_executable(thr_hello thr_hello.c)
target_link_libraries(thr_hello pthread)

add_executable(signals signals.c)
target_link_libraries(signals pthread)


get_property(TARGETS
	DIRECTORY .
	PROPERTY BUILDSYSTEM_TARGETS)

# deploy all targets with install and also provide uninstall target
deploy("${TARGETS}" ${HERMIT_BIN_DIR} tests)

if(TARGET hermit)
	# add dependency on hermit, so that everything will be built *after*
	# libhermit in order to link against the freshly compiled version
	foreach(TARGET ${TARGETS})
		add_dependencies(${TARGET} hermit)
	endforeach()
endif()
