cmake_minimum_required(VERSION 3.7)
include(../../cmake/HermitCore.cmake)

project(hermit_benchmarks C)

add_compile_options(${HERMIT_APP_FLAGS})

add_executable(basic basic.c)
target_link_libraries(basic pthread)

add_executable(hg hg.c hist.c rdtsc.c run.c init.c opt.c report.c setup.c)

add_executable(netio netio.c)

add_executable(RCCE_pingpong RCCE_pingpong.c)
target_link_libraries(RCCE_pingpong ircce)

add_executable(stream stream.c)
target_compile_options(stream PRIVATE -fopenmp)
target_link_libraries(stream iomp pthread)

set(TARGETS
	basic hg netio RCCE_pingpong stream)

add_custom_target(hermit_benchmarks
    DEPENDS ${TARGETS})

install(
	TARGETS
		${TARGETS}
	DESTINATION
		${HERMIT_BIN_DIR})
