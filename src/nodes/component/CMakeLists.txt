project(component_nodes VERSION 1.0.0)
set(TARGET_NAME ${TARGET_PREFIX}${PROJECT_NAME})
add_definitions(-DTARGET_NAME=${TARGET_NAME})
add_definitions(-DPROJECT_VERSION=${PROJECT_VERSION})

# Project Block0 IDs : Decimals [1249248 - 1249263], Hexadecimals [0x00130fe0 - 0x00130fef]
set(PROJECT_BLOCK0_ID_MIN "0x00130fe0")
set(PROJECT_BLOCK0_ID_MAX "0x00130fef")
set(${PROJECT_NAME}_BLOCK0_RESERVED_STATE FALSE CACHE BOOL "Block0 reserve state")
if (NOT ${${PROJECT_NAME}_BLOCK0_RESERVED_STATE})
	RESERVE_ID_BLOCK(${PROJECT_BLOCK0_ID_MIN} ${PROJECT_BLOCK0_ID_MAX})
	set(${PROJECT_NAME}_BLOCK0_RESERVED_STATE TRUE CACHE BOOL "Block0 reserve state" FORCE)
endif()
mark_as_advanced(${PROJECT_NAME}_BLOCK0_RESERVED_STATE)

# Project Block1 IDs : Decimals [1249264 - 1249279], Hexadecimals [0x00130ff0 - 0x00130fff]
set(PROJECT_BLOCK1_ID_MIN "0x00130ff0")
set(PROJECT_BLOCK1_ID_MAX "0x00130fff")
set(${PROJECT_NAME}_BLOCK1_RESERVED_STATE FALSE CACHE BOOL "Block1 reserve state")
if (NOT ${${PROJECT_NAME}_BLOCK1_RESERVED_STATE})
	RESERVE_ID_BLOCK(${PROJECT_BLOCK1_ID_MIN} ${PROJECT_BLOCK1_ID_MAX})
	set(${PROJECT_NAME}_BLOCK1_RESERVED_STATE TRUE CACHE BOOL "Block1 reserve state" FORCE)
endif()
mark_as_advanced(${PROJECT_NAME}_BLOCK1_RESERVED_STATE)

# Project Block2 IDs : Decimals [1249280 - 1249295], Hexadecimals [0x00131000 - 0x0013100f]
set(PROJECT_BLOCK2_ID_MIN "0x00131000")
set(PROJECT_BLOCK2_ID_MAX "0x0013100f")
set(${PROJECT_NAME}_BLOCK2_RESERVED_STATE FALSE CACHE BOOL "Block2 reserve state")
if (NOT ${${PROJECT_NAME}_BLOCK2_RESERVED_STATE})
	RESERVE_ID_BLOCK(${PROJECT_BLOCK2_ID_MIN} ${PROJECT_BLOCK2_ID_MAX})
	set(${PROJECT_NAME}_BLOCK2_RESERVED_STATE TRUE CACHE BOOL "Block2 reserve state" FORCE)
endif()
mark_as_advanced(${PROJECT_NAME}_BLOCK2_RESERVED_STATE)

# Preprocessor definition - cache containing all reserved ids within the assigned project id blocks
string(REPLACE ";" "," _TMP "${${PROJECT_NAME}_ID_CACHE}")
add_definitions("-DPROJECT_ID_CACHE={${_TMP}}")

# Files
set(CPP_FILES		
	"${CMAKE_CURRENT_SOURCE_DIR}/aim_transform.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/aim_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/footRoll_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/flexiSpine_locator.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/flexiSpine_locator_scaleAdjustment_manip.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/flexiSpine_locator_twistAdjustment_manip.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/flexiSpine_locator_upVector_manip.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/flexiSpine_locator_subSceneOverride.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/flexiInstancer_locator.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/flexiInstancer_locator_positionAdjustment_manip.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/flexiInstancer_locator_scaleAdjustment_manip.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/flexiInstancer_locator_twistAdjustment_manip.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/flexiInstancer_locator_upVector_manip.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/flexiInstancer_locator_subSceneOverride.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/flexiChainSingle_locator.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/flexiChainSingle_locator_scaleAdjustment_manip.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/flexiChainSingle_locator_twistAdjustment_manip.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/flexiChainSingle_locator_upVector_manip.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/flexiChainSingle_locator_subSceneOverride.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/flexiChainDouble_locator.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/flexiChainDouble_locator_scaleAdjustment_manip.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/flexiChainDouble_locator_twistAdjustment_manip.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/flexiChainDouble_locator_upVector_manip.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/flexiChainDouble_locator_subSceneOverride.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/flexiChainTriple_locator.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/flexiChainTriple_locator_scaleAdjustment_manip.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/flexiChainTriple_locator_twistAdjustment_manip.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/flexiChainTriple_locator_upVector_manip.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/flexiChainTriple_locator_subSceneOverride.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vChainSolver_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vChainPlanarSolver_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/plugin.cpp")

set(MEL_SCRIPT_FILES	
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/shaders/MRS_findTexture2D.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/shaders/MRS_texture2D.xml"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/callbacks/MRS_FlexiSpine_rmbCallback.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/callbacks/MRS_FlexiInstancer_rmbCallback.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/callbacks/MRS_FlexiChainSingle_rmbCallback.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/callbacks/MRS_FlexiChainDouble_rmbCallback.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/callbacks/MRS_FlexiChainTriple_rmbCallback.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiSpine_contextTools.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiSpine_counterTwistUI.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiSpine_debugUpVectorUI.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiSpine_scaleAdjustmentUINew.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiSpine_scaleAdjustmentUIReplace.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiSpine_twistAdjustmentUINew.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiSpine_twistAdjustmentUIReplace.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiInstancer_contextTools.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiInstancer_counterTwistUI.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiInstancer_debugUpVectorUI.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiInstancer_positionAdjustmentUINew.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiInstancer_positionAdjustmentUIReplace.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiInstancer_scaleAdjustmentUINew.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiInstancer_scaleAdjustmentUIReplace.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiInstancer_twistAdjustmentUINew.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiInstancer_twistAdjustmentUIReplace.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiChainSingle_contextTools.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiChainSingle_counterTwistUI.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiChainSingle_debugUpVectorUI.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiChainSingle_scaleAdjustmentUINew.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiChainSingle_scaleAdjustmentUIReplace.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiChainSingle_twistAdjustmentUINew.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiChainSingle_twistAdjustmentUIReplace.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiChainDouble_contextTools.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiChainDouble_counterTwistUI.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiChainDouble_debugUpVectorUI.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiChainDouble_scaleAdjustmentUINew.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiChainDouble_scaleAdjustmentUIReplace.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiChainDouble_twistAdjustmentUINew.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiChainDouble_twistAdjustmentUIReplace.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiChainTriple_contextTools.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiChainTriple_counterTwistUI.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiChainTriple_debugUpVectorUI.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiChainTriple_scaleAdjustmentUINew.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiChainTriple_scaleAdjustmentUIReplace.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiChainTriple_twistAdjustmentUINew.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiChainTriple_twistAdjustmentUIReplace.mel")

set(PYTHON_SCRIPT_FILES
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiSpine_scaleAdjustmentUIDock.py"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiSpine_twistAdjustmentUIDock.py"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiInstancer_positionAdjustmentUIDock.py"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiInstancer_scaleAdjustmentUIDock.py"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiInstancer_twistAdjustmentUIDock.py"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiChainSingle_scaleAdjustmentUIDock.py"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiChainSingle_twistAdjustmentUIDock.py"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiChainDouble_scaleAdjustmentUIDock.py"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiChainDouble_twistAdjustmentUIDock.py"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiChainTriple_scaleAdjustmentUIDock.py"
	"${CMAKE_CURRENT_SOURCE_DIR}/scripts/tools/MRS_FlexiChainTriple_twistAdjustmentUIDock.py")

# Target
add_library(${TARGET_NAME} SHARED ${CPP_FILES})
# Target - Link utils_static
target_link_libraries(${TARGET_NAME} PRIVATE ${TARGET_PREFIX}utils_static ${TARGET_PREFIX}data_static) # utils_static dependencies (eg. Maya) are transitive (ie. interface is inherited)
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

# Install MEL Scripts
install(FILES ${MEL_SCRIPT_FILES}
	DESTINATION "${DIST_INSTALL_DIR}/module/scripts/mel")

# Install Python Scripts
install(FILES ${PYTHON_SCRIPT_FILES}
	DESTINATION "${DIST_INSTALL_DIR}/module/scripts/python")

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