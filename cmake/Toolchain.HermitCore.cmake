set(TOOLCHAIN_DIR /opt/hermit)
set(TARGET x86_64-hermit)

set(CMAKE_SYSTEM_NAME Generic)

# CMake will also use the path to search for binutils and C++ compiler
set(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}/bin/${TARGET}-gcc)

# apparently cmake will also look for libraries there
#set(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_DIR}/${TARGET})
