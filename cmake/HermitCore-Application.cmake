include(${CMAKE_CURRENT_LIST_DIR}/HermitCore.cmake)

add_compile_options(${HERMIT_APP_FLAGS})

# link against local libhermit.a instead of the one supplied with the
# toolchain, if built from top-level
link_directories(${LOCAL_PREFIX_ARCH_LIB_DIR})

