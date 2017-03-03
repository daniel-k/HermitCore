cmake_minimum_required(VERSION 3.7)

include(ExternalProject)
include(cmake/HermitCore.cmake)

project (HermitCore)

### Kernel

# generate config.h from template, then generate config.inc from config.h
# exports: config-h, config-inc
add_subdirectory(include/hermit)

add_kernel_module_sources("kernel"		"kernel/*.c")
add_kernel_module_sources("libkern"		"libkern/*.c")
add_kernel_module_sources("mm"			"mm/*.c")
add_kernel_module_sources("drivers"		"drivers/net/*.c")

set(LWIP_SRC lwip/src)
add_kernel_module_sources("lwip"	"${LWIP_SRC}/api/*.c")
add_kernel_module_sources("lwip"	"${LWIP_SRC}/arch/*.c")
add_kernel_module_sources("lwip"	"${LWIP_SRC}/core/*.c")
add_kernel_module_sources("lwip"	"${LWIP_SRC}/core/ipv4/*.c")
add_kernel_module_sources("lwip"	"${LWIP_SRC}/core/ipv6/*.c")
add_kernel_module_sources("lwip"	"${LWIP_SRC}/netif/*.c")

get_kernel_modules(KERNEL_MODULES)
foreach(MODULE ${KERNEL_MODULES})
	get_kernel_module_sources(SOURCES ${MODULE})

	# maintain list of all objects that will end up in libhermit.a
	list(APPEND KERNEL_OBJECTS $<TARGET_OBJECTS:${MODULE}>)

	add_library(${MODULE} OBJECT ${SOURCES})

	# this is kernel code
	target_compile_definitions(${MODULE}
		PRIVATE -D__KERNEL__)

	target_compile_options(${MODULE}
		PRIVATE ${HERMIT_KERNEL_FLAGS})

	target_include_directories(${MODULE}
		PUBLIC ${HERMIT_KERNEL_INCLUDES})

	# kernel code probably needs global config.h
	add_dependencies(${MODULE} config-h)

	# suppress all LwIP compiler warnings. Not our code, so we cannot fix
	if("${MODULE}" STREQUAL "lwip")
		target_compile_options(${MODULE}
			PRIVATE -w)
	endif()

endforeach()

# add arch/x86 and its objects
# TODO: make this conditional when new architectures are implemented
add_subdirectory(arch/x86)
list(APPEND KERNEL_OBJECTS
	$<TARGET_OBJECTS:${X86_KERNEL_ASM_TARGET}>
	$<TARGET_OBJECTS:${X86_KERNEL_C_TARGET}>)

add_dependencies(${X86_KERNEL_ASM_TARGET} config-inc)
add_dependencies(${X86_KERNEL_C_TARGET} config-h)

# finally build libhermit.a
add_library(hermit STATIC ${KERNEL_OBJECTS})

# after compiling ASM sources, we need to post-process them. Adding this
# dependency makes sure that this is done before hermit is linked
add_dependencies(hermit ${X86_KERNEL_TARGET})

# rename sections in final library
add_custom_command(
	TARGET
		hermit POST_BUILD
	COMMAND
		${CMAKE_OBJCOPY} --rename-section .bss=.kbss
						 --rename-section .text=.ktext
						 --rename-section .data=.kdata
						 $<TARGET_FILE:hermit>)


### External projects

# Intel's OpenMP runtime for x86
ExternalProject_Add(libiomp
	SOURCE_DIR ${HERMIT_ROOT}/usr/libomp
	# install to intermediate third-party libs location
	INSTALL_COMMAND
		${CMAKE_COMMAND} --build <BINARY_DIR>
		                 --target install --
		                     DESTDIR=${EXTERNALS_INTERMEDIATE_DIR}
	CMAKE_ARGS
		-DHERMIT=1
		-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
		-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX})

add_dependencies(hermit libiomp)

# relocate external libraries to our install destination
install(DIRECTORY ${EXTERNALS_BASE_DIR}
	DESTINATION ${CMAKE_INSTALL_PREFIX}/${TARGET_ARCH})


### Examples
# include applications at the end so they can depend on libhermit and other
# external runtimes that are freshly built

add_subdirectory(usr/benchmarks)
add_subdirectory(usr/tests)
