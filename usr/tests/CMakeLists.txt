cmake_minimum_required(VERSION 3.7.1)
include(../../cmake/HermitCore.cmake)

project(hermit_tests C CXX Fortran Go)

add_compile_options(${HERMIT_APP_FLAGS})

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

install(TARGETS
			hello hello++ hellof pi server RCCE_minimum thr_hello
			signals jacobi
		DESTINATION
			${HERMIT_BIN_DIR}/tests)

