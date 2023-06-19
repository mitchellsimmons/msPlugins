project(public_commands VERSION 1.0.0)
set(TARGET_NAME ${TARGET_PREFIX}${PROJECT_NAME})
add_definitions(-DTARGET_NAME=${TARGET_NAME})
add_definitions(-DPROJECT_VERSION=${PROJECT_VERSION})

# Files
set(CPP_FILES	
	"${CMAKE_CURRENT_SOURCE_DIR}/instancerBake_cmd.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/plugin.cpp")

# Target
add_library(${TARGET_NAME} SHARED "${CPP_FILES}")
# Target - Link utils_static
target_link_libraries(${TARGET_NAME} PRIVATE ${TARGET_PREFIX}utils_static) # utils_static dependencies (eg. Maya) are transitive (ie. interface is inherited)
# Target - Set Properties (INTERFACE settings such as INTERFACE_COMPILE_DEFINITIONS are inherited from Maya)
set_target_properties(${TARGET_NAME} PROPERTIES
	PREFIX ""
    SUFFIX ${MAYA_PLUGIN_EXTENSION}
	DEBUG_POSTFIX "_D"
	FOLDER "commands") # VS - Organize project into a folder
# Target - Symbol Exports
if(WIN32)
	set_target_properties(${TARGET_NAME} PROPERTIES
        LINK_FLAGS "/export:initializePlugin /export:uninitializePlugin")
endif()

# Install - data
install(TARGETS ${TARGET_NAME}
	LIBRARY DESTINATION "${DIST_INSTALL_DIR}/module/plugins/${MAYA_VERSION}/dependencies"
	RUNTIME DESTINATION "${DIST_INSTALL_DIR}/module/plugins/${MAYA_VERSION}/dependencies")