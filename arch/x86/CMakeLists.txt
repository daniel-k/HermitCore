cmake_minimum_required(VERSION 3.7.1)
include(../../cmake/HermitCore.cmake)

project(arch_x86_kernel C ASM_NASM)

# compiling kernel code here
add_definitions(-D__KERNEL__)

add_custom_target(${PROJECT_NAME})

### ASM sources ###

add_library(${PROJECT_NAME}_asm OBJECT
	kernel/entry.asm
	kernel/string.asm)

# HACK: We need to post-process the objects by running elfedit on them, but
#       there is currently no way to get the list of objects out of CMake
#       except for $<TARGET_OBJECTS:tgt>, which only works with add_library()
#       and add_executable().
#       So predict path to objects and add custom commands that depend on
#       the asm target.
#
#       Upstream issue: https://gitlab.kitware.com/cmake/cmake/issues/15226
#
set(_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_FILES_DIRECTORY}")
set(_BUILD_DIR "${_BUILD_DIR}/${PROJECT_NAME}_asm.dir")

get_target_property(ASM_SOURCES ${PROJECT_NAME}_asm SOURCES)
foreach(SOURCE ${ASM_SOURCES})
	set(OBJECT "${SOURCE}.obj")
	set(OBJECT_PATH "${_BUILD_DIR}/${OBJECT}")

	# slash (/) not allowed in target names
	string(REPLACE "/" "-"
		OBJECT_TARGET_NAME
		"${OBJECT}")

	add_custom_target("${OBJECT_TARGET_NAME}"
		COMMAND
			${CMAKE_ELFEDIT} --output-osabi HermitCore ${OBJECT_PATH}
		DEPENDS
			${PROJECT_NAME}_asm)

	# make main target depend on this
	add_dependencies(${PROJECT_NAME} ${OBJECT_TARGET_NAME})
endforeach()


### C sources ###

file(GLOB KERNEL_SOURCES	"kernel/*.c")
file(GLOB MM_SOURCES		"mm/*.c")

# add boot.h as source to mark dependency boot.asm -> boot.h -> apic.c
add_library(${PROJECT_NAME}_c OBJECT
	${KERNEL_SOURCES} ${MM_SOURCES}
	${CMAKE_BINARY_DIR}/boot.h)

target_include_directories(${PROJECT_NAME}_c BEFORE
	PUBLIC ${HERMIT_KERNEL_INCLUDES}
	PRIVATE ${CMAKE_BINARY_DIR})

target_compile_options(${PROJECT_NAME}_c
	PRIVATE ${HERMIT_KERNEL_FLAGS})

# assemble boot.asm and dump to C-array in boot.h
add_custom_command(
	OUTPUT
		${CMAKE_BINARY_DIR}/boot.h
	DEPENDS
		kernel/boot.asm
	COMMAND
		echo "static const uint8_t boot_code[] = {" > boot.h
	COMMAND
		nasm -f bin -o boot.bin ${CMAKE_CURRENT_LIST_DIR}/kernel/boot.asm
	COMMAND
		hexdump -v -e "7/1 \"0x%02X, \" 1/1 \" 0x%02X,\\n\"" boot.bin >> boot.h
	COMMAND
		echo "};" >> boot.h
	WORKING_DIRECTORY
		${CMAKE_BINARY_DIR}/
	VERBATIM USES_TERMINAL)
