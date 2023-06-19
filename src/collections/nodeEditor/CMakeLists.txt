# We are defining a collection as a standalone project containing a group of inter-related plugins (eg. nodes, commands, etc)
project(nodeEditor_collection VERSION 1.0.0)
# In this case our collection is designed for private use, therefore all plugins will receive a baked prefix
set(TARGET_NAME mrs_${PROJECT_NAME})
add_definitions(-DTARGET_NAME=${TARGET_NAME})
add_definitions(-DPROJECT_VERSION=${PROJECT_VERSION})

# Project Block0 IDs : Decimals [1249312 - 1249327], Hexadecimals [0x00131020 - 0x0013102f]
set(PROJECT_BLOCK0_ID_MIN "0x00131020")
set(PROJECT_BLOCK0_ID_MAX "0x0013102f")
set(${PROJECT_NAME}_BLOCK0_RESERVED_STATE FALSE CACHE BOOL "Block0 reserve state")
if (NOT ${${PROJECT_NAME}_BLOCK0_RESERVED_STATE})
	RESERVE_ID_BLOCK(${PROJECT_BLOCK0_ID_MIN} ${PROJECT_BLOCK0_ID_MAX})
	set(${PROJECT_NAME}_BLOCK0_RESERVED_STATE TRUE CACHE BOOL "Block0 reserve state" FORCE)
endif()
mark_as_advanced(${PROJECT_NAME}_BLOCK0_RESERVED_STATE)

# Preprocessor definition - cache containing all reserved ids within the assigned project id blocks
string(REPLACE ";" "," _TMP "${${PROJECT_NAME}_ID_CACHE}")
add_definitions("-DPROJECT_ID_CACHE={${_TMP}}")

# Files
set(CPP_FILES
	"${CMAKE_CURRENT_SOURCE_DIR}/nodeBoxContainer_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/stickyContainer_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/nodeEditorTabInfoMetadata_cmd.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/plugin.cpp")

# Target
add_library(${TARGET_NAME} SHARED ${CPP_FILES})
# Target - Link utils_static
target_link_libraries(${TARGET_NAME} PRIVATE ${TARGET_PREFIX}utils_static) # utils_static dependencies (ie. Maya) are transitive
# Target - Set Properties (INTERFACE settings such as INTERFACE_COMPILE_DEFINITIONS are inherited from Maya)
set_target_properties(${TARGET_NAME} PROPERTIES
	PREFIX ""
    SUFFIX ${MAYA_PLUGIN_EXTENSION}
	DEBUG_POSTFIX "_D"
	FOLDER "collections") # VS - Organize project into a folder
# Target - Symbol Exports
if(WIN32)
	set_target_properties(${TARGET_NAME} PROPERTIES
        LINK_FLAGS "/export:initializePlugin /export:uninitializePlugin")
endif()

# Install - Targets
install(TARGETS ${TARGET_NAME}
	LIBRARY DESTINATION "${DIST_INSTALL_DIR}/module/plugins/${MAYA_VERSION}"
	RUNTIME DESTINATION "${DIST_INSTALL_DIR}/module/plugins/${MAYA_VERSION}")