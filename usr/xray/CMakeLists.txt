cmake_minimum_required(VERSION 3.7)
include(../../cmake/HermitCore.cmake)

project(hermit_xray C)

add_compile_options(${HERMIT_APP_FLAGS})

file(GLOB SOURCES *.c)

add_library(xray STATIC ${SOURCES})

target_compile_definitions(xray
	PUBLIC
		-DXRAY -DXRAY_ANNOTATE
		-DXRAY_NO_DEMANGLE
		-DXRAY_DISABLE_BROWSER_INTEGRATION)

# deploy all targets with install and also provide uninstall target
deploy(xray ${TARGET_ARCH}/lib xray)
install(FILES libxray.spec
		DESTINATION ${TARGET_ARCH}/lib)
install(FILES xray.h
		DESTINATION ${TARGET_ARCH}/include)
