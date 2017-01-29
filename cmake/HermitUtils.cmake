macro(add_kernel_module_sources MODULE SOURCE_GLOB)
	file(GLOB SOURCES "${SOURCE_GLOB}")

    if("${SOURCES}" STREQUAL "")
        message(FATAL_ERROR "Module '${MODULE}' has no sources")
    endif()

    # make sure modules are unique, this is needed of multiple sources
    # are added to the same module
    list(APPEND _KERNEL_MODULES "${MODULE}")
    list(REMOVE_DUPLICATES _KERNEL_MODULES)

    # append sources for module
    list(APPEND "_KERNEL_SOURCES_${MODULE}" "${SOURCES}")
endmacro(add_kernel_module_sources)

macro(get_kernel_module_sources VAR MODULE)
    set(${VAR} ${_KERNEL_SOURCES_${MODULE}})
endmacro(get_kernel_module_sources)

macro(get_kernel_modules VAR)
	set(${VAR} ${_KERNEL_MODULES})
endmacro(get_kernel_modules)

# find program in /toolchain/dir/prefix-NAME, only supply NAME
macro(find_toolchain_program NAME)

    string(TOUPPER "${NAME}" NAME_UPPER)
    string(TOLOWER "${NAME}" NAME_LOWER)

    set(VARNAME "CMAKE_${NAME_UPPER}")

    find_program(${VARNAME}
        NAMES ${_CMAKE_TOOLCHAIN_PREFIX}${NAME_LOWER}
        HINTS ${TOOLCHAIN_BIN_DIR})

    if(NOT ${VARNAME})
        message(FATAL_ERROR
				"Cannot find ${_CMAKE_TOOLCHAIN_PREFIX}${NAME_LOWER}")
    endif()
endmacro(find_toolchain_program)

macro(deploy TARGETS DESTINATION COMPONENT)
	install(TARGETS ${TARGETS}
		DESTINATION ${DESTINATION}/${COMPONENT}
		COMPONENT   ${COMPONENT})

	set(UNINSTALL_TARGET uninstall-${COMPONENT})

	add_custom_target(${UNINSTALL_TARGET}
		COMMAND
			${CMAKE_COMMAND} -E remove_directory ${DESTINATION}/${COMPONENT}
		COMMAND
			rmdir --ignore-fail-on-non-empty ${DESTINATION})

	if(NOT TARGET uninstall)
		add_custom_target(uninstall)
	endif()

	add_dependencies(uninstall ${UNINSTALL_TARGET})
endmacro(deploy)
