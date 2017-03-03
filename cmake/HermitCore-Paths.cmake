# root of HermitCore project
set(HERMIT_ROOT ${CMAKE_CURRENT_LIST_DIR}/..)

# where to install compiled binaries
set(HERMIT_BIN_DIR ${HERMIT_ROOT}/bin)

# we define our own install prefix
set(CMAKE_INSTALL_PREFIX /opt/hermit)

# we install 3rd party libraries to an intermediate directory and relocate them
# later when installing the whole project
set(EXTERNALS_INTERMEDIATE_DIR ${CMAKE_BINARY_DIR}/externals)
set(EXTERNALS_BASE_DIR ${EXTERNALS_INTERMEDIATE_DIR}/${CMAKE_INSTALL_PREFIX})
set(EXTERNALS_LIB_DIR ${EXTERNALS_BASE_DIR}/lib)
