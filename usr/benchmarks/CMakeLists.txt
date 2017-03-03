cmake_minimum_required(VERSION 3.7)
include(../../cmake/HermitCore.cmake)

project(hermit_benchmarks C)

add_compile_options(${HERMIT_APP_FLAGS})

# link against local libhermit.a instead of the one supplied with the
# toolchain, if built from top-level
link_directories(${CMAKE_BINARY_DIR} ${EXTERNALS_LIB_DIR})

add_executable(basic basic.c)
target_link_libraries(basic pthread)

add_executable(hg hg.c hist.c rdtsc.c run.c init.c opt.c report.c setup.c)

add_executable(netio netio.c)

add_executable(RCCE_pingpong RCCE_pingpong.c)
target_link_libraries(RCCE_pingpong ircce)

add_executable(stream stream.c)
target_compile_options(stream PRIVATE -fopenmp)
target_link_libraries(stream -fopenmp)

get_property(TARGETS
    DIRECTORY .
    PROPERTY BUILDSYSTEM_TARGETS)

# deploy all targets with install and also provide uninstall target
deploy("${TARGETS}" ${HERMIT_BIN_DIR} benchmarks)

if(TARGET hermit)
	# add dependency on hermit, so that everything will be built *after*
	# libhermit in order to link against the freshly compiled version
	foreach(TARGET ${TARGETS})
		add_dependencies(${TARGET} hermit)
	endforeach()
endif()
