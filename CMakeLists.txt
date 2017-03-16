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

# deploy libhermit.a and its headers
deploy(hermit ${TARGET_ARCH}/lib libhermit)
install(DIRECTORY ${HERMIT_ROOT}/include/hermit
	DESTINATION ${TARGET_ARCH}/include/
	FILES_MATCHING PATTERN "*.h")


### External projects
#
# Build projects externally and deploy into temporary common prefix, will later
# be relocated for installation.

## HermitCore's own tools such as Qemu/KVM proxy
build_external(tools ${HERMIT_ROOT}/tools "")

## Intel's OpenMP runtime for x86 (libomp)
build_external(libiomp ${HERMIT_ROOT}/usr/libomp ""
		-DHERMIT=1
		-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
		-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/${TARGET_ARCH})

# libomp is part of HermitCore's runtime and should be available before any
# application will link
add_dependencies(hermit libiomp)

## iRCCE
build_external(ircce ${HERMIT_ROOT}/usr/ircce "")
add_dependencies(hermit ircce)

## XRay profiler
build_external(xray ${HERMIT_ROOT}/usr/xray "")
add_dependencies(hermit xray)

## Tests and benchmarks
build_external(tests ${HERMIT_ROOT}/usr/tests hermit)
build_external(benchmarks ${HERMIT_ROOT}/usr/benchmarks hermit)
build_external(openmpbench ${HERMIT_ROOT}/usr/openmpbench hermit)

# relocate external libraries to our install destination
install(DIRECTORY ${EXTERNALS_PREFIX_DIR}/
	DESTINATION ${CMAKE_INSTALL_PREFIX}/
	USE_SOURCE_PERMISSIONS)


### Packaging

set(CPACK_PACKAGE_NAME libhermit)
set(CPACK_SYSTEM_NAME all)

set(CPACK_PACKAGE_VERSION_MAJOR 0)
set(CPACK_PACKAGE_VERSION_MINOR 1)
set(CPACK_PACKAGE_VERSION_PATCH 0)

set(CPACK_PACKAGE_CONTACT "Daniel Krebs <github@daniel-krebs.net>")

# build .deb, .rpm and .tar.bz2 packages
set(CPACK_GENERATOR DEB;RPM;TBZ2)

# needed in order for tests and bechmark to use correct install prefix
set(CPACK_SET_DESTDIR on)

## Debian specific
# not dependent on Debian system architecture
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE all)

## RPM specific
# libhermit is currently not relocatable
set(CPACK_PACKAGE_RELOCATABLE FALSE)

include(CPack)
