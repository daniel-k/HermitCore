cmake_minimum_required(VERSION 3.7.1)

set(CMAKE_TOOLCHAIN_FILE ../../cmake/Toolchain.HermitCore.cmake)
project (hermit_tests)

enable_language(C)
enable_language(CXX)
enable_language(Fortran)
enable_language(Go)

add_executable(hello hello.c)
add_executable(hello++ hello++.cpp)
add_executable(hellof hellof.f90)

add_executable(pi pi.go)
add_executable(server server.go)

target_link_libraries(server netgo)

install(TARGETS hello hello++ DESTINATION ${PROJECT_SOURCE_DIR}/bin)

