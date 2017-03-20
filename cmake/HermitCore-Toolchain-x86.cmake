include(${CMAKE_CURRENT_LIST_DIR}/HermitCore-Utils.cmake)
include_guard()

# let user provide a different path to the toolchain
set_default(TOOLCHAIN_BIN_DIR /opt/hermit/bin)

set(TARGET_ARCH x86_64-hermit)

set(CMAKE_SYSTEM_NAME Generic)

# point CMake to our toolchain, it will auto detect C++ compiler
set(CMAKE_C_COMPILER ${TOOLCHAIN_BIN_DIR}/${TARGET_ARCH}-gcc)
set(CMAKE_Fortran_COMPILER ${TOOLCHAIN_BIN_DIR}/${TARGET_ARCH}-gfortran)
set(CMAKE_Go_COMPILER ${TOOLCHAIN_BIN_DIR}/${TARGET_ARCH}-gccgo)

# hinting the prefix and location is needed in order to correctly detect
# binutils
set(_CMAKE_TOOLCHAIN_PREFIX "${TARGET_ARCH}-")
set(_CMAKE_TOOLCHAIN_LOCATION ${TOOLCHAIN_BIN_DIR})
