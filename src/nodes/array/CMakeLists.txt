project(array_nodes VERSION 1.0.0)
set(TARGET_NAME ${TARGET_PREFIX}${PROJECT_NAME})
add_definitions(-DTARGET_NAME=${TARGET_NAME})
add_definitions(-DPROJECT_VERSION=${PROJECT_VERSION})

# Project Block0 IDs : Decimals [1249232 - 1249247], Hexadecimals [0x00130fd0 - 0x00130fdf]
set(PROJECT_BLOCK0_ID_MIN "0x00130fd0")
set(PROJECT_BLOCK0_ID_MAX "0x00130fdf")
set(${PROJECT_NAME}_BLOCK0_RESERVED_STATE FALSE CACHE BOOL "Block0 reserve state")
if (NOT ${${PROJECT_NAME}_BLOCK0_RESERVED_STATE})
	RESERVE_ID_BLOCK(${PROJECT_BLOCK0_ID_MIN} ${PROJECT_BLOCK0_ID_MAX})
	set(${PROJECT_NAME}_BLOCK0_RESERVED_STATE TRUE CACHE BOOL "Block0 reserve state" FORCE)
endif()
mark_as_advanced(${PROJECT_NAME}_BLOCK0_RESERVED_STATE)

# Project Block1 IDs : Decimals [1249328 - 1249343], Hexadecimals [0x00131030 - 0x0013103f]
set(PROJECT_BLOCK1_ID_MIN "0x00131030")
set(PROJECT_BLOCK1_ID_MAX "0x0013103f")
set(${PROJECT_NAME}_BLOCK1_RESERVED_STATE FALSE CACHE BOOL "Block1 reserve state")
if (NOT ${${PROJECT_NAME}_BLOCK1_RESERVED_STATE})
	RESERVE_ID_BLOCK(${PROJECT_BLOCK1_ID_MIN} ${PROJECT_BLOCK1_ID_MAX})
	set(${PROJECT_NAME}_BLOCK1_RESERVED_STATE TRUE CACHE BOOL "Block1 reserve state" FORCE)
endif()
mark_as_advanced(${PROJECT_NAME}_BLOCK1_RESERVED_STATE)

# Preprocessor definition - cache containing all reserved ids within the assigned project id blocks
string(REPLACE ";" "," _TMP "${${PROJECT_NAME}_ID_CACHE}")
add_definitions("-DPROJECT_ID_CACHE={${_TMP}}")

# Variables
set(CPP_FILES		
	"${CMAKE_CURRENT_SOURCE_DIR}/angle/packAngleArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/angle/unpackAngleArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/angle/arithmeticSequenceAngleArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/angle/geometricSequenceAngleArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/double/packDoubleArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/double/unpackDoubleArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/double/arithmeticSequenceDoubleArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/double/geometricSequenceDoubleArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/int/packIntArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/int/unpackIntArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/int/arithmeticSequenceIntArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/int/geometricSequenceIntArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/packMatrixArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/unpackMatrixArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/composeMatrixArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/decomposeMatrixArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector/packVectorArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector/unpackVectorArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector/arithmeticSequenceVectorArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector/geometricSequenceVectorArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/euler/packEulerArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/euler/unpackEulerArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/euler/arithmeticSequenceEulerArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/euler/geometricSequenceEulerArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/quaternion/packQuaternionArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/quaternion/unpackQuaternionArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/plugin.cpp")

set(INCLUDE_DIRS	
	"${CMAKE_CURRENT_SOURCE_DIR}/angle"
	"${CMAKE_CURRENT_SOURCE_DIR}/double"
	"${CMAKE_CURRENT_SOURCE_DIR}/int"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector"
	"${CMAKE_CURRENT_SOURCE_DIR}/euler"
	"${CMAKE_CURRENT_SOURCE_DIR}/quaternion")

# Target
add_library(${TARGET_NAME} SHARED "${CPP_FILES}")
# Target - Link utils_static
target_link_libraries(${TARGET_NAME} PUBLIC ${TARGET_PREFIX}utils_static ${TARGET_PREFIX}data_static) # utils_static dependencies (eg. Maya) are transitive (ie. interface is inherited)
# Target - Set Properties (INTERFACE settings such as INTERFACE_COMPILE_DEFINITIONS are inherited from Maya)
set_target_properties(${TARGET_NAME} PROPERTIES
	INCLUDE_DIRECTORIES "${INCLUDE_DIRS}" # Same as: target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/matrix;...)
	PREFIX ""
    SUFFIX ${MAYA_PLUGIN_EXTENSION}
	DEBUG_POSTFIX "_D"
	FOLDER "nodes") # VS - Organize project into a folder
# Target - Symbol Exports
if(WIN32)
	set_target_properties(${TARGET_NAME} PROPERTIES
        LINK_FLAGS "/export:initializePlugin /export:uninitializePlugin")
endif()

# Install - Targets
install(TARGETS ${TARGET_NAME}
	LIBRARY DESTINATION "${DIST_INSTALL_DIR}/module/plugins/${MAYA_VERSION}"
	RUNTIME DESTINATION "${DIST_INSTALL_DIR}/module/plugins/${MAYA_VERSION}")

# Install - Templates
add_custom_target(${TARGET_NAME}_templates ALL
    COMMAND ${CMAKE_COMMAND} 
	-DPROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}" -DNODE_NAME_PREFIX="${NODE_NAME_PREFIX}" -DDIST_INSTALL_DIR="${DIST_INSTALL_DIR}"
	-P "${CMAKE_CURRENT_SOURCE_DIR}/installTemplates.cmake")

set_target_properties(${TARGET_NAME}_templates PROPERTIES
	FOLDER "templates")