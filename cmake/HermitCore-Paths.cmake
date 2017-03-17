# root of HermitCore project
set(HERMIT_ROOT ${CMAKE_CURRENT_LIST_DIR}/..)

# where to install compiled binaries
set(HERMIT_BIN_DIR ${HERMIT_ROOT}/bin)

# define our own install prefix
set(CMAKE_INSTALL_PREFIX /opt/hermit)

# we install 3rd party libraries to an intermediate directory and relocate
# them here later when installing the whole project
if(NOT LOCAL_PREFIX_BASE_DIR)
	# will be injected into external project because CMAKE_BINARY_DIR will be
	# different there
	set(LOCAL_PREFIX_BASE_DIR ${CMAKE_BINARY_DIR}/local_prefix)
endif()

# during build process libraries and external projects will be deployed into
# this directory structure
set(LOCAL_PREFIX_DIR ${LOCAL_PREFIX_BASE_DIR}/${CMAKE_INSTALL_PREFIX})
set(LOCAL_PREFIX_ARCH_DIR ${LOCAL_PREFIX_DIR}/${TARGET_ARCH})

# when building applications within the HermitCore project (tests, ...) they
# will link prefarably against libraries in this directory in order to test
# changes in the kernel
set(LOCAL_PREFIX_ARCH_LIB_DIR ${LOCAL_PREFIX_ARCH_DIR}/lib)
