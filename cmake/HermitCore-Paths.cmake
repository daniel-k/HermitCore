# root of HermitCore project
set(HERMIT_ROOT ${CMAKE_CURRENT_LIST_DIR}/..)

# where to install compiled binaries
set(HERMIT_BIN_DIR ${HERMIT_ROOT}/bin)

# we define our own install prefix
set(CMAKE_INSTALL_PREFIX /opt/hermit)

# we install 3rd party libraries to an intermediate directory and relocate them
# later when installing the whole project
if(NOT EXTERNALS_BASE_DIR)
	# will be injected into external project because CMAKE_BINARY_DIR will be
	# different there
	set(EXTERNALS_BASE_DIR ${CMAKE_BINARY_DIR}/externals)
endif()
set(EXTERNALS_PREFIX_DIR ${EXTERNALS_BASE_DIR}/${CMAKE_INSTALL_PREFIX})
set(EXTERNALS_LIB_DIR ${EXTERNALS_PREFIX_DIR}/lib)
set(EXTERNALS_LIB_ARCH_DIR ${EXTERNALS_PREFIX_DIR}/${TARGET_ARCH}/lib)
