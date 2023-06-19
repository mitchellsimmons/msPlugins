project(data VERSION 1.0.0)
set(STATIC_TARGET_NAME ${TARGET_PREFIX}${PROJECT_NAME}_static)
set(SHARED_TARGET_NAME ${TARGET_PREFIX}${PROJECT_NAME})
add_definitions(-DTARGET_NAME=${SHARED_TARGET_NAME})
add_definitions(-DPROJECT_VERSION=${PROJECT_VERSION})

# Project Block0 IDs : Decimals [1249216 - 1249231], Hexadecimals [0x00130fc0 - 0x00130fcf]
set(PROJECT_BLOCK0_ID_MIN "0x00130fc0")
set(PROJECT_BLOCK0_ID_MAX "0x00130fcf")
set(${PROJECT_NAME}_BLOCK0_RESERVED_STATE FALSE CACHE BOOL "Block0 reserve state")
if (NOT ${${PROJECT_NAME}_BLOCK0_RESERVED_STATE})
	RESERVE_ID_BLOCK(${PROJECT_BLOCK0_ID_MIN} ${PROJECT_BLOCK0_ID_MAX})
	set(${PROJECT_NAME}_BLOCK0_RESERVED_STATE TRUE CACHE BOOL "Block0 reserve state" FORCE)
endif()
mark_as_advanced(${PROJECT_NAME}_BLOCK0_RESERVED_STATE)

# Preprocessor definition - cache containing all reserved ids within the assigned project id blocks
string(REPLACE ";" "," _TMP "${${PROJECT_NAME}_ID_CACHE}")
add_definitions("-DPROJECT_ID_CACHE={${_TMP}}")

# Variables
set(CPP_FILES_STATIC	"${CMAKE_CURRENT_SOURCE_DIR}/angleArray_data.cpp"
						"${CMAKE_CURRENT_SOURCE_DIR}/eulerArray_data.cpp"
						"${CMAKE_CURRENT_SOURCE_DIR}/quaternionArray_data.cpp")
set(CPP_FILES_SHARED	${CPP_FILES_STATIC}
						"${CMAKE_CURRENT_SOURCE_DIR}/plugin.cpp")

# Target - STATIC
add_library(${STATIC_TARGET_NAME} STATIC ${CPP_FILES_STATIC})
target_link_libraries(${STATIC_TARGET_NAME} PRIVATE ${TARGET_PREFIX}utils_static)
set_target_properties(${STATIC_TARGET_NAME} PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/.."  # Ensures depending targets have access when target_link_libraries(consumer, data) is called
	FOLDER "archive")

# Target - SHARED
add_library(${SHARED_TARGET_NAME} SHARED ${CPP_FILES_SHARED})
# Target - Link utils_static
target_link_libraries(${SHARED_TARGET_NAME} PRIVATE ${TARGET_PREFIX}utils_static) # utils_static dependencies (ie. Maya) are transitive
# Target - Set Properties (INTERFACE settings such as INTERFACE_COMPILE_DEFINITIONS are inherited from Maya)
set_target_properties(${SHARED_TARGET_NAME} PROPERTIES
	PREFIX ""
    SUFFIX ${MAYA_PLUGIN_EXTENSION}
	DEBUG_POSTFIX "_D"
	FOLDER "data")
# Target - Symbol Exports
if(WIN32)
	set_target_properties(${SHARED_TARGET_NAME} PROPERTIES
        LINK_FLAGS "/export:initializePlugin /export:uninitializePlugin")
endif()

# Install - Targets
install(TARGETS ${SHARED_TARGET_NAME}
	LIBRARY DESTINATION "${DIST_INSTALL_DIR}/module/plugins/${MAYA_VERSION}/dependencies"
	RUNTIME DESTINATION "${DIST_INSTALL_DIR}/module/plugins/${MAYA_VERSION}/dependencies")