cmake_minimum_required(VERSION 3.7.1)

include(../../../cmake/HermitCore.cmake)

project (arch_x86_kernel C ASM_NASM)

# compiling kernel code here
add_definitions(-D__KERNEL__)

### C sources ###

file(GLOB_RECURSE C_FILES "*.c")

add_library(arch_x86_kernel_c OBJECT ${C_FILES})

target_include_directories(arch_x86_kernel_c BEFORE
	PUBLIC ${HERMIT_KERNEL_INCLUDES})

target_compile_options(arch_x86_kernel_c PRIVATE ${HERMIT_KERNEL_FLAGS})

# apic.c depends on boot code from boot.asm which shall be converted
# to a C-array that can be included via boot.h
set_source_files_properties(apic.c PROPERTIES OBJECT_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/boot.h)

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
		${CMAKE_CURRENT_SOURCE_DIR}
	VERBATIM USES_TERMINAL)


### ASM sources ###
add_library(arch_x86_kernel_asm OBJECT entry.asm string.asm)


# combine to one target that will be access from top-level
add_library(arch_x86_kernel STATIC
	$<TARGET_OBJECTS:arch_x86_kernel_asm>
	$<TARGET_OBJECTS:arch_x86_kernel_c>)
