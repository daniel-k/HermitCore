cmake_minimum_required(VERSION 3.7)
include(cmake/HermitCore.cmake)

project (HermitCore)

### Kernel

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
	$<TARGET_OBJECTS:arch_x86_kernel_c>
	$<TARGET_OBJECTS:arch_x86_kernel_asm>)

# finally build libhermit.a
add_library(hermit STATIC ${KERNEL_OBJECTS})

# after compiling ASM sources, we need to post-process them. Adding this
# dependency makes sure that this is done before hermit is linked
add_dependencies(hermit arch_x86_kernel)

# rename sections in final library
add_custom_command(
	TARGET
		hermit POST_BUILD
	COMMAND
		${CMAKE_OBJCOPY} --rename-section .bss=.kbss
						 --rename-section .text=.ktext
						 --rename-section .data=.kdata
						 $<TARGET_FILE:hermit>)


### Examples
# add after kernel, so they can depend on libhermit

add_subdirectory(usr/benchmarks)
add_subdirectory(usr/tests)
