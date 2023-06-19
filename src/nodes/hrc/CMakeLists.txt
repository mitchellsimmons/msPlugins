project(hrc_nodes VERSION 1.0.0)
set(TARGET_NAME ${TARGET_PREFIX}${PROJECT_NAME})
add_definitions(-DTARGET_NAME=${TARGET_NAME})
add_definitions(-DPROJECT_VERSION=${PROJECT_VERSION})

# Project Block0 IDs : Decimals [1249520 - 1249535], Hexadecimals [0x001310f0 - 0x001310ff]
set(PROJECT_BLOCK0_ID_MIN "0x001310f0")
set(PROJECT_BLOCK0_ID_MAX "0x001310ff")
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
	"${CMAKE_CURRENT_SOURCE_DIR}/hrc.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/hrc_matrix.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/module_hrc.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/rig_hrc.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/plugin.cpp")

# Target
add_library(${TARGET_NAME} SHARED ${CPP_FILES})
# Target - Link utils_static
target_link_libraries(${TARGET_NAME} PRIVATE ${TARGET_PREFIX}utils_static) # utils_static dependencies (eg. Maya) are transitive (ie. interface is inherited)
# Target - Set Properties (INTERFACE settings such as INTERFACE_COMPILE_DEFINITIONS are inherited from Maya)
set_target_properties(${TARGET_NAME} PROPERTIES
	PREFIX ""
    SUFFIX ${MAYA_PLUGIN_EXTENSION}
	DEBUG_POSTFIX "_D"
	FOLDER "nodes") # VS - Organize project into a folder
# Target - Symbol Exports
if(WIN32)
	set_target_properties(${TARGET_NAME} PROPERTIES
        LINK_FLAGS "/export:initializePlugin /export:uninitializePlugin")
endif()

# Install Target
install(TARGETS ${TARGET_NAME}
	LIBRARY DESTINATION "${DIST_INSTALL_DIR}/module/plugins/${MAYA_VERSION}"
	RUNTIME DESTINATION "${DIST_INSTALL_DIR}/module/plugins/${MAYA_VERSION}")

# Install Templates
# We could also use add_custom_command(TARGET ${TARGET_NAME} PRE_BUILD COMMAND ...)
# However this would only run when building the target (reinstalling targets would not reinstall the templates unless the target needed to be rebuilt)
# Therefore if the user deletes the dist folder before reinstalling, the second install would not contain the templates
# Using add_custom_target() ensures that our command will always be executed, therefore our templates will always be installed
add_custom_target(${TARGET_NAME}_templates ALL
    COMMAND ${CMAKE_COMMAND} 
	-DTEMPLATE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/scripts/templates" -DNODE_NAME_PREFIX="${NODE_NAME_PREFIX}" -DDIST_INSTALL_DIR="${DIST_INSTALL_DIR}"
	-P "${CMAKE_CURRENT_SOURCE_DIR}/installTemplates.cmake")

set_target_properties(${TARGET_NAME}_templates PROPERTIES
	FOLDER "templates")