cmake_minimum_required(VERSION 3.7.1)
include(../cmake/HermitCore.cmake)

project(kernel C)

# build all .c files in current directory
include(../cmake/HermitKernelC.cmake)
