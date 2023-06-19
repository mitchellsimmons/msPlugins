cmake_minimum_required(VERSION 3.11)
project(msPlugins VERSION 1.0.0)

# Flags - compilation
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# VS - Organize projects for pre-defined targets into a folder
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Find dependencies

# The below package is a config-file package (cmake searches for these in CMAKE_PREFIX_PATH so we must update this to include the config file path)
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/dependencies/SeExpr/share/cmake/SeExpr2")
find_package(seexpr2 REQUIRED)

# The below package is a find-module package (cmake searches for these in CMAKE_MODULE_PATH so we must update this to include the find module path)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
# Find MAYA package (load variables from FindMaya module)
find_package(MAYA REQUIRED)
# Include helpers
include(hex)

# This is a pure template library (ie. no config required)
set(Eigen_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/dependencies")

# Variables - installation (defaults)
if(NOT DEFINED LIB_INSTALL_DIR)
    set(LIB_INSTALL_DIR "${CMAKE_SOURCE_DIR}/lib")
endif()

if(NOT DEFINED INCLUDE_INSTALL_DIR)
    set(INCLUDE_INSTALL_DIR "${CMAKE_SOURCE_DIR}/include")
endif()

if(NOT DEFINED DIST_INSTALL_DIR)
    set(DIST_INSTALL_DIR "${CMAKE_SOURCE_DIR}/dist")
endif()

if(NOT DEFINED NODE_NAME_PREFIX)
	set(NODE_NAME_PREFIX "mrs_")
endif()

set(TARGET_PREFIX "mrs_")

# Variables - ids
# Global Block0 IDs : Decimals [1249216 - 1249727], Hexadecimals [0x00130fc0 - 0x001311bf]
# Global Block0 Owner : Mitchell Simmons
set(GLOBAL_BLOCK0_ID_MIN "0x00130fc0")
set(GLOBAL_BLOCK0_ID_MAX "0x001311bf")

# Variables - build config
set(BUILD_NODES, OFF)
set(BUILD_NODES_ARRAY, OFF)
set(BUILD_NODES_COMPONENT, OFF)
set(BUILD_NODES_HRC, OFF)
set(BUILD_NODES_MATH, OFF)
set(DEVELOPER_BUILD, OFF)
set(INSTALL_INTERNAL_TARGETS, OFF)

# Preprocessor definitions - available to the compiler command line for targets in the current directory
# As of Visual Studio 15 2017, if we add a definition surrounded by quotations, the compiler seems to treat it as a string literal however IntelliSense will just view the value as an identifier
# To have more consistent behaviour, add an unquoted definition and then stringize the macro from within the source code
add_definitions(-DNODE_NAME_PREFIX=${NODE_NAME_PREFIX})
add_definitions(-D_USE_MATH_DEFINES)

# Function - reserve ids on a per-project basis from a global block (safeguards against conflicting project ids)
function(RESERVE_ID_BLOCK MIN_HEX MAX_HEX)
	from_hex(${MIN_HEX} _min)
	from_hex(${MAX_HEX} _max)
	from_hex(${GLOBAL_BLOCK0_ID_MIN} _globalMin)
	from_hex(${GLOBAL_BLOCK0_ID_MAX} _globalMax)
	foreach(_id RANGE ${_min} ${_max})
		foreach(_reservedId ${GLOBAL_ID_CACHE})
			if (${_id} EQUAL ${_reservedId})
				message(FATAL_ERROR "${_id} : Maya ID is already registered, resolve conflict then rebuild.")
			endif()
		endforeach()

		if (${_id} LESS ${_globalMin} OR ${_id} GREATER ${_globalMax})
			message(FATAL_ERROR "${_id} : Maya ID is outside of the global ID range, resolve issue then rebuild.")
		endif()

		# Variable should persist between runs
		set(GLOBAL_ID_CACHE ${GLOBAL_ID_CACHE} ${_id} CACHE INTERNAL "Maya IDs reserved accross all projects for all runs")
		set(${PROJECT_NAME}_ID_CACHE ${${PROJECT_NAME}_ID_CACHE} ${_id} CACHE INTERNAL "Maya IDs reserved for current project")
	endforeach()
endfunction()

# Target - utils (all plugins depend on this library)
add_subdirectory(src/utils)

# Target - data (only installed when there is a dependent plugin)
if(BUILD_NODES OR BUILD_NODES_ARRAY)
	add_subdirectory(src/data)
endif()

# Target - nodes
if(BUILD_NODES)
	add_subdirectory(src/nodes/array)
	add_subdirectory(src/nodes/component)
	add_subdirectory(src/nodes/hrc)
	add_subdirectory(src/nodes/math)
else()
	if(BUILD_NODES_ARRAY)
		add_subdirectory(src/nodes/array)
	endif(BUILD_NODES_ARRAY)

	if(BUILD_NODES_COMPONENT)
		add_subdirectory(src/nodes/component)
	endif(BUILD_NODES_COMPONENT)

	if(BUILD_NODES_HRC)
		add_subdirectory(src/nodes/hrc)
	endif(BUILD_NODES_HRC)

	if(BUILD_NODES_MATH)
		add_subdirectory(src/nodes/math)
	endif(BUILD_NODES_MATH)
endif(BUILD_NODES)

# Target - private collections (only install for developer)
if (DEVELOPER_BUILD)
	add_subdirectory(src/collections/nodeEditor)
endif()

# Target - private_commands (only install when there is a dependent plugin)
if(BUILD_NODES OR BUILD_NODES_COMPONENT)
	add_subdirectory(src/commands/private)
	add_subdirectory(src/commands/public)
endif()

# Install - script dependencies (generic scripts used by plugin scripts)
add_subdirectory(src/nodes/scripts)

# Install - Templates
add_custom_target(module_templates ALL
    COMMAND ${CMAKE_COMMAND} 
	-DTEMPLATE_DIR="${CMAKE_SOURCE_DIR}/scripts/templates" -DDIST_INSTALL_DIR="${DIST_INSTALL_DIR}" -DPROJECT_VERSION="${CMAKE_PROJECT_VERSION}" -DMAYA_VERSION="${MAYA_VERSION}"
	-P "${CMAKE_SOURCE_DIR}/scripts/installTemplates.cmake")

set_target_properties(module_templates PROPERTIES
	FOLDER "templates")

# Debug messages
list(SORT GLOBAL_ID_CACHE)
message("Reserved IDs : ${GLOBAL_ID_CACHE}")

list(LENGTH GLOBAL_ID_CACHE _GLOBAL_ID_CACHE_LENGTH)
math(EXPR _GLOBAL_ID_CACHE_END_INDEX "${_GLOBAL_ID_CACHE_LENGTH} - 1")
list(GET GLOBAL_ID_CACHE ${_GLOBAL_ID_CACHE_END_INDEX} _GLOBAL_ID_CACHE_END)
math(EXPR _GLOBAL_ID_CACHE_AVAILABLE_START "${_GLOBAL_ID_CACHE_END} + 1")
math(EXPR _GLOBAL_ID_CACHE_AVAILABLE_END "${_GLOBAL_ID_CACHE_AVAILABLE_START} + 15")

if (${_GLOBAL_ID_CACHE_AVAILABLE_START} GREATER ${GLOBAL_BLOCK0_ID_MAX})
	message("Global ID Block 0 : Completely reserved")
else()
	message("Next available ID block : [${_GLOBAL_ID_CACHE_AVAILABLE_START} - ${_GLOBAL_ID_CACHE_AVAILABLE_END}]")
endif()