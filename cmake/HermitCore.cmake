# scripts to detect HermitCore Go compiler
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/golang/)

set(HERMIT_ROOT ${CMAKE_CURRENT_LIST_DIR}/..)
set(HERMIT_BIN_DIR ${HERMIT_ROOT}/bin)

if(NOT HERMIT_ARCH)
	set(HERMIT_ARCH x86)
endif()
message("HermitCore architecture is: ${HERMIT_ARCH}")

# use default toolchain if not specified by user
if(NOT CMAKE_TOOLCHAIN_FILE)
	set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/Toolchain.HermitCore.cmake)
endif()

# NASM detection will change binary format depending on host system, but
# we only want to generate elf64 for HermitCore
# Note: Has to be set *before* ASM_NASM is enabled
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)

enable_language(ASM_NASM)

# NASM hack, because it requires include paths to have a trailing /, whereas
# CMake explicitly will remove it when adding includes the usual way
# Note: Has to be set *after* ASM_NASM is enabled
set(CMAKE_ASM_NASM_FLAGS
	"${CMAKE_ASM_NASM_FLAGS} -I ${HERMIT_ROOT}/include/hermit/")

set(HERMIT_KERNEL_FLAGS
					-g -m64 -Wall -O2 -mno-red-zone
					-fno-var-tracking-assignments -fstrength-reduce
					-fomit-frame-pointer -finline-functions -ffreestanding
					-nostdinc -fno-stack-protector -mno-sse -mno-mmx
					-mno-sse2 -mno-3dnow -mno-avx
					-fno-delete-null-pointer-checks
					-falign-jumps=1 -falign-loops=1
					-mno-80387 -mno-fp-ret-in-387 -mskip-rax-setup
					-fno-common -Wframe-larger-than=1024
					-fno-strict-aliasing -fno-asynchronous-unwind-tables
					-fno-strict-overflow -maccumulate-outgoing-args)

set(HERMIT_APP_FLAGS
					-m64 -mtls-direct-seg-refs -O3 -ftree-vectorize)

set(HERMIT_KERNEL_INCLUDES
    ${HERMIT_ROOT}/include
    ${HERMIT_ROOT}/arch/${HERMIT_ARCH}/include
    ${HERMIT_ROOT}/lwip/src/include
    ${HERMIT_ROOT}/drivers)
