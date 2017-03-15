cmake_minimum_required(VERSION 3.7)
include(../../cmake/HermitCore.cmake)

project(hermit_openmpbench C)

add_compile_options(${HERMIT_APP_FLAGS})

# link against local libhermit.a instead of the one supplied with the
# toolchain, if built from top-level
link_directories(${CMAKE_BINARY_DIR} ${EXTERNALS_LIB_DIR})

add_executable(syncbench syncbench.c common.c)
target_link_libraries(syncbench PUBLIC -fopenmp)

add_executable(taskbench taskbench.c common.c)
target_link_libraries(taskbench PUBLIC -fopenmp)

add_library(common_sched STATIC common.c)
target_compile_definitions(common_sched PUBLIC -DSCHEDBENCH)
target_compile_options(common_sched PUBLIC -fopenmp)
target_link_libraries(common_sched PUBLIC -fopenmp)

add_executable(schedbench schedbench.c)
target_link_libraries(schedbench common_sched)

get_property(TARGETS
    DIRECTORY .
    PROPERTY BUILDSYSTEM_TARGETS)

# do not deploy common_schedbench
list(REMOVE_ITEM TARGETS common_sched)

# deploy all targets with install and also provide uninstall target
deploy("${TARGETS}" extra/benchmarks openmpbench)

if(TARGET hermit)
	# add dependency on hermit, so that everything will be built *after*
	# libhermit in order to link against the freshly compiled version
	foreach(TARGET ${TARGETS})
		add_dependencies(${TARGET} hermit)
	endforeach()
endif()
