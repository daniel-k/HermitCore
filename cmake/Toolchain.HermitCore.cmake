set(TOOLCHAIN_DIR /opt/hermit)
set(TARGET_ARCH x86_64-hermit)

set(CMAKE_SYSTEM_NAME Generic)

# point CMake to our toolchain, it will auto detect C++ compiler
set(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}/bin/${TARGET_ARCH}-gcc)
set(CMAKE_Fortran_COMPILER ${TOOLCHAIN_DIR}/bin/${TARGET_ARCH}-gfortran)
set(CMAKE_Go_COMPILER ${TOOLCHAIN_DIR}/bin/${TARGET_ARCH}-gccgo)

# hinting the prefix is needed in order to correctly detect binutils
set(_CMAKE_TOOLCHAIN_PREFIX "${TARGET_ARCH}-")
