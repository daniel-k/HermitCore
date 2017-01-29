cmake_minimum_required(VERSION 3.7)

set(CMAKE_TOOLCHAIN_FILE ../../cmake/Toolchain.HermitCore.cmake)
project (hermit_benchmarks)

add_executable(basic basic.c)
target_link_libraries(basic pthread)

add_executable(hg hg.c hist.c rdtsc.c run.c init.c opt.c report.c setup.c)

add_executable(netio netio.c)

add_executable(RCCE_pingpong RCCE_pingpong.c)
target_link_libraries(RCCE_pingpong ircce)

add_executable(stream stream.c)
target_compile_options(stream PRIVATE -fopenmp)
target_link_libraries(stream iomp pthread)

install(TARGETS
			basic hg netio RCCE_pingpong stream
		DESTINATION
			${HERMIT_ROOT}/bin)
