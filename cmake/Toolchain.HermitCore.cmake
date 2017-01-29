set(TOOLCHAIN_DIR /opt/hermit)
set(TARGET x86_64-hermit)

set(CMAKE_SYSTEM_NAME Generic)

# point CMake to our toolchain, it will auto detect binutils and C++ compiler
set(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}/bin/${TARGET}-gcc)
set(CMAKE_Fortran_COMPILER ${TOOLCHAIN_DIR}/bin/${TARGET}-gfortran)
set(CMAKE_Go_COMPILER ${TOOLCHAIN_DIR}/bin/${TARGET}-gccgo)
