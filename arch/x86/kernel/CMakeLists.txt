cmake_minimum_required(VERSION 3.7.1)

set(CMAKE_TOOLCHAIN_FILE ../../../cmake/Toolchain.HermitCore.cmake)
project (arch_x86_kernel C ASM_NASM)

# move this to somewhere else, nasm hack
add_compile_options(-I ${HERMIT_ROOT}/include/hermit/ )

# compiling kernel code here
add_definitions(-D__KERNEL__)

include_directories(
	${HERMIT_ROOT}/include
	${HERMIT_ROOT}/arch/x86/include
	${HERMIT_ROOT}/lwip/src/include
	${HERMIT_ROOT}/drivers)

file(GLOB_RECURSE C_FILES "*.c")

# make sure that NASM creates elf64 format, no matter on which host system
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
enable_language(ASM_NASM)

# create kernel library
add_library(arch_x86_kernel entry.asm string.asm ${C_FILES})

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
