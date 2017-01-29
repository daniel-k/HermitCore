# Compile all C files in current directory as kernel and create library
# target with name of project.
#
# Example: kernel/CMakeLists.txt
#
# project(kernel C)
# include(../cmake/HermitKernelC.cmake)
#

# compiling kernel code here
add_definitions(-D__KERNEL__)

### C sources ###

file(GLOB C_SOURCES "*.c")

add_library(${PROJECT_NAME} STATIC ${C_SOURCES})

target_compile_options(${PROJECT_NAME}
	PRIVATE ${HERMIT_KERNEL_FLAGS})

target_include_directories(${PROJECT_NAME} BEFORE
	PUBLIC ${HERMIT_KERNEL_INCLUDES})

