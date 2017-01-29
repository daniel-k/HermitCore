cmake_minimum_required(VERSION 3.7.1)
include(../../cmake/HermitCore.cmake)

project(arch_x86_kernel C ASM_NASM)

# compiling kernel code here
add_definitions(-D__KERNEL__)


### ASM sources ###

add_library(${PROJECT_NAME}_asm OBJECT
	kernel/entry.asm
	kernel/string.asm)


### C sources ###

file(GLOB KERNEL_SOURCES	"kernel/*.c")
file(GLOB MM_SOURCES		"mm/*.c")

add_library(${PROJECT_NAME}_c OBJECT
	${KERNEL_SOURCES} ${MM_SOURCES})

target_include_directories(${PROJECT_NAME}_c BEFORE
	PUBLIC ${HERMIT_KERNEL_INCLUDES})

target_compile_options(${PROJECT_NAME}_c
	PRIVATE ${HERMIT_KERNEL_FLAGS})

# apic.c depends on boot code from boot.asm which shall be converted
# to a C-array that can be included via boot.h
set_source_files_properties(apic.c
	PROPERTIES
		OBJECT_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/boot.h)

# assemble boot.asm and dump to C-array in boot.h
add_custom_command(
	OUTPUT
		boot.h
	DEPENDS
		boot.asm
	COMMAND
		echo "static const uint8_t boot_code[] = {" > boot.h
	COMMAND
		nasm -f bin -o boot.bin boot.asm
	COMMAND
		hexdump -v -e "7/1 \"0x%02X, \" 1/1 \" 0x%02X,\\n\"" boot.bin >> boot.h
	COMMAND
		echo "};" >> boot.h
	COMMAND
		rm boot.bin
	WORKING_DIRECTORY
		${CMAKE_CURRENT_LIST_DIR}/kernel
	VERBATIM USES_TERMINAL)
