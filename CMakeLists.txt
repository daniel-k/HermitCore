cmake_minimum_required(VERSION 3.7.1)

set(CMAKE_TOOLCHAIN_FILE cmake/Toolchain.HermitCore.cmake)
project (HermitCore)

add_subdirectory(usr/benchmarks)
add_subdirectory(usr/tests)
