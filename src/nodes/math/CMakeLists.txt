project(math_nodes VERSION 1.1.0)
set(TARGET_NAME ${TARGET_PREFIX}${PROJECT_NAME})
add_definitions(-DTARGET_NAME=${TARGET_NAME})
add_definitions(-DPROJECT_VERSION=${PROJECT_VERSION})

# Project Block0 IDs : Decimals [1249296 - 1249311], Hexadecimals [0x00131010 - 0x0013101f]
set(PROJECT_BLOCK0_ID_MIN "0x00131010")
set(PROJECT_BLOCK0_ID_MAX "0x0013101f")
set(${PROJECT_NAME}_BLOCK0_RESERVED_STATE FALSE CACHE BOOL "Block0 reserve state")
if (NOT ${${PROJECT_NAME}_BLOCK0_RESERVED_STATE})
	RESERVE_ID_BLOCK(${PROJECT_BLOCK0_ID_MIN} ${PROJECT_BLOCK0_ID_MAX})
	set(${PROJECT_NAME}_BLOCK0_RESERVED_STATE TRUE CACHE BOOL "Block0 reserve state" FORCE)
endif()
mark_as_advanced(${PROJECT_NAME}_BLOCK0_RESERVED_STATE)

# Project Block1 IDs : Decimals [1249344 - 1249359], Hexadecimals [0x00131040 - 0x0013104f]
set(PROJECT_BLOCK1_ID_MIN "0x00131040")
set(PROJECT_BLOCK1_ID_MAX "0x0013104f")
set(${PROJECT_NAME}_BLOCK1_RESERVED_STATE FALSE CACHE BOOL "Block1 reserve state")
if (NOT ${${PROJECT_NAME}_BLOCK1_RESERVED_STATE})
	RESERVE_ID_BLOCK(${PROJECT_BLOCK1_ID_MIN} ${PROJECT_BLOCK1_ID_MAX})
	set(${PROJECT_NAME}_BLOCK1_RESERVED_STATE TRUE CACHE BOOL "Block1 reserve state" FORCE)
endif()
mark_as_advanced(${PROJECT_NAME}_BLOCK1_RESERVED_STATE)

# Project Block2 IDs : Decimals [1249360 - 1249375], Hexadecimals [0x00131050 - 0x0013105f]
set(PROJECT_BLOCK2_ID_MIN "0x00131050")
set(PROJECT_BLOCK2_ID_MAX "0x0013105f")
set(${PROJECT_NAME}_BLOCK2_RESERVED_STATE FALSE CACHE BOOL "Block2 reserve state")
if (NOT ${${PROJECT_NAME}_BLOCK2_RESERVED_STATE})
	RESERVE_ID_BLOCK(${PROJECT_BLOCK2_ID_MIN} ${PROJECT_BLOCK2_ID_MAX})
	set(${PROJECT_NAME}_BLOCK2_RESERVED_STATE TRUE CACHE BOOL "Block2 reserve state" FORCE)
endif()
mark_as_advanced(${PROJECT_NAME}_BLOCK2_RESERVED_STATE)

# Project Block3 IDs : Decimals [1249376 - 1249391], Hexadecimals [0x00131060 - 0x0013106f]
set(PROJECT_BLOCK3_ID_MIN "0x00131060")
set(PROJECT_BLOCK3_ID_MAX "0x0013106f")
set(${PROJECT_NAME}_BLOCK3_RESERVED_STATE FALSE CACHE BOOL "Block3 reserve state")
if (NOT ${${PROJECT_NAME}_BLOCK3_RESERVED_STATE})
	RESERVE_ID_BLOCK(${PROJECT_BLOCK3_ID_MIN} ${PROJECT_BLOCK3_ID_MAX})
	set(${PROJECT_NAME}_BLOCK3_RESERVED_STATE TRUE CACHE BOOL "Block3 reserve state" FORCE)
endif()
mark_as_advanced(${PROJECT_NAME}_BLOCK3_RESERVED_STATE)

# Project Block4 IDs : Decimals [1249392 - 1249407], Hexadecimals [0x00131070 - 0x0013107f]
set(PROJECT_BLOCK4_ID_MIN "0x00131070")
set(PROJECT_BLOCK4_ID_MAX "0x0013107f")
set(${PROJECT_NAME}_BLOCK4_RESERVED_STATE FALSE CACHE BOOL "Block4 reserve state")
if (NOT ${${PROJECT_NAME}_BLOCK4_RESERVED_STATE})
	RESERVE_ID_BLOCK(${PROJECT_BLOCK4_ID_MIN} ${PROJECT_BLOCK4_ID_MAX})
	set(${PROJECT_NAME}_BLOCK4_RESERVED_STATE TRUE CACHE BOOL "Block4 reserve state" FORCE)
endif()
mark_as_advanced(${PROJECT_NAME}_BLOCK4_RESERVED_STATE)

# Project Block5 IDs : Decimals [1249408 - 1249423], Hexadecimals [0x00131080 - 0x0013108f]
set(PROJECT_BLOCK5_ID_MIN "0x00131080")
set(PROJECT_BLOCK5_ID_MAX "0x0013108f")
set(${PROJECT_NAME}_BLOCK5_RESERVED_STATE FALSE CACHE BOOL "Block5 reserve state")
if (NOT ${${PROJECT_NAME}_BLOCK5_RESERVED_STATE})
	RESERVE_ID_BLOCK(${PROJECT_BLOCK5_ID_MIN} ${PROJECT_BLOCK5_ID_MAX})
	set(${PROJECT_NAME}_BLOCK5_RESERVED_STATE TRUE CACHE BOOL "Block5 reserve state" FORCE)
endif()
mark_as_advanced(${PROJECT_NAME}_BLOCK5_RESERVED_STATE)

# Project Block6 IDs : Decimals [1249424 - 1249439], Hexadecimals [0x00131090 - 0x0013109f]
set(PROJECT_BLOCK6_ID_MIN "0x00131090")
set(PROJECT_BLOCK6_ID_MAX "0x0013109f")
set(${PROJECT_NAME}_BLOCK6_RESERVED_STATE FALSE CACHE BOOL "Block6 reserve state")
if (NOT ${${PROJECT_NAME}_BLOCK6_RESERVED_STATE})
	RESERVE_ID_BLOCK(${PROJECT_BLOCK6_ID_MIN} ${PROJECT_BLOCK6_ID_MAX})
	set(${PROJECT_NAME}_BLOCK6_RESERVED_STATE TRUE CACHE BOOL "Block6 reserve state" FORCE)
endif()
mark_as_advanced(${PROJECT_NAME}_BLOCK6_RESERVED_STATE)

# Project Block7 IDs : Decimals [1249440 - 1249455], Hexadecimals [0x001310a0 - 0x001310af]
set(PROJECT_BLOCK7_ID_MIN "0x001310a0")
set(PROJECT_BLOCK7_ID_MAX "0x001310af")
set(${PROJECT_NAME}_BLOCK7_RESERVED_STATE FALSE CACHE BOOL "Block7 reserve state")
if (NOT ${${PROJECT_NAME}_BLOCK7_RESERVED_STATE})
	RESERVE_ID_BLOCK(${PROJECT_BLOCK7_ID_MIN} ${PROJECT_BLOCK7_ID_MAX})
	set(${PROJECT_NAME}_BLOCK7_RESERVED_STATE TRUE CACHE BOOL "Block7 reserve state" FORCE)
endif()
mark_as_advanced(${PROJECT_NAME}_BLOCK7_RESERVED_STATE)

# Project Block8 IDs : Decimals [1249456 - 1249471], Hexadecimals [0x001310b0 - 0x001310bf]
set(PROJECT_BLOCK8_ID_MIN "0x001310b0")
set(PROJECT_BLOCK8_ID_MAX "0x001310bf")
set(${PROJECT_NAME}_BLOCK8_RESERVED_STATE FALSE CACHE BOOL "Block8 reserve state")
if (NOT ${${PROJECT_NAME}_BLOCK8_RESERVED_STATE})
	RESERVE_ID_BLOCK(${PROJECT_BLOCK8_ID_MIN} ${PROJECT_BLOCK8_ID_MAX})
	set(${PROJECT_NAME}_BLOCK8_RESERVED_STATE TRUE CACHE BOOL "Block8 reserve state" FORCE)
endif()
mark_as_advanced(${PROJECT_NAME}_BLOCK8_RESERVED_STATE)

# Project Block9 IDs : Decimals [1249472 - 1249487], Hexadecimals [0x001310c0 - 0x001310cf]
set(PROJECT_BLOCK9_ID_MIN "0x001310c0")
set(PROJECT_BLOCK9_ID_MAX "0x001310cf")
set(${PROJECT_NAME}_BLOCK9_RESERVED_STATE FALSE CACHE BOOL "Block9 reserve state")
if (NOT ${${PROJECT_NAME}_BLOCK9_RESERVED_STATE})
	RESERVE_ID_BLOCK(${PROJECT_BLOCK9_ID_MIN} ${PROJECT_BLOCK9_ID_MAX})
	set(${PROJECT_NAME}_BLOCK9_RESERVED_STATE TRUE CACHE BOOL "Block9 reserve state" FORCE)
endif()
mark_as_advanced(${PROJECT_NAME}_BLOCK9_RESERVED_STATE)

# Project Block10 IDs : Decimals [1249488 - 1249503], Hexadecimals [0x001310d0 - 0x001310df]
set(PROJECT_BLOCK10_ID_MIN "0x001310d0")
set(PROJECT_BLOCK10_ID_MAX "0x001310df")
set(${PROJECT_NAME}_BLOCK10_RESERVED_STATE FALSE CACHE BOOL "Block10 reserve state")
if (NOT ${${PROJECT_NAME}_BLOCK10_RESERVED_STATE})
	RESERVE_ID_BLOCK(${PROJECT_BLOCK10_ID_MIN} ${PROJECT_BLOCK10_ID_MAX})
	set(${PROJECT_NAME}_BLOCK10_RESERVED_STATE TRUE CACHE BOOL "Block10 reserve state" FORCE)
endif()
mark_as_advanced(${PROJECT_NAME}_BLOCK10_RESERVED_STATE)

# Project Block11 IDs : Decimals [1249504 - 1249519], Hexadecimals [0x001310e0 - 0x001310ef]
set(PROJECT_BLOCK11_ID_MIN "0x001310e0")
set(PROJECT_BLOCK11_ID_MAX "0x001310ef")
set(${PROJECT_NAME}_BLOCK11_RESERVED_STATE FALSE CACHE BOOL "Block11 reserve state")
if (NOT ${${PROJECT_NAME}_BLOCK11_RESERVED_STATE})
	RESERVE_ID_BLOCK(${PROJECT_BLOCK11_ID_MIN} ${PROJECT_BLOCK11_ID_MAX})
	set(${PROJECT_NAME}_BLOCK11_RESERVED_STATE TRUE CACHE BOOL "Block11 reserve state" FORCE)
endif()
mark_as_advanced(${PROJECT_NAME}_BLOCK11_RESERVED_STATE)

# Preprocessor definition - cache containing all reserved ids within the assigned project id blocks
string(REPLACE ";" "," _TMP "${${PROJECT_NAME}_ID_CACHE}")
add_definitions("-DPROJECT_ID_CACHE={${_TMP}}")

# Variables
set(CPP_FILES
	# Arithmetic
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/addAngle_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/add_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/addInt_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/averageAngle_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/average_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/averageInt_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/divideAngle_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/divideAngleByInt_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/divide_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/divideByInt_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/divideInt_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/modulus_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/multiplyAngle_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/multiplyAngleByInt_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/multiply_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/multiplyByInt_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/multiplyInt_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/negateAngle_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/negate_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/negateInt_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/normalizeDoubleArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/normalizeWeightArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/reciprocal_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/subtractAngle_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/subtract_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/subtractInt_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/sumAngle_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/sum_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/sumInt_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/weightedAverageAngle_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/weightedAverage_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic/weightedAverageInt_node.cpp"
	# Condition
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/condition_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/conditionAngle_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/conditionInt_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/max_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/maxElement_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/maxAngle_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/maxElementAngle_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/maxInt_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/maxElementInt_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/min_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/minElement_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/minAngle_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/minElementAngle_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/minInt_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/minElementInt_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/select_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/selectAngle_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/selectAngleArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/selectCurve_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/selectDoubleArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/selectEuler_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/selectEulerArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/selectInt_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/selectIntArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/selectMatrix_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/selectMatrixArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/selectMesh_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/selectQuaternion_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/selectQuaternionArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/selectSurface_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/selectVector_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition/selectVectorArray_node.cpp"
	# Distance
	"${CMAKE_CURRENT_SOURCE_DIR}/distance/absolute_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/distance/absoluteAngle_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/distance/absoluteInt_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/distance/distanceBetweenFrames_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/distance/distanceBetweenPoints_node.cpp"
	# Euler
	"${CMAKE_CURRENT_SOURCE_DIR}/euler/averageEuler_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/euler/convertEulerToMatrix_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/euler/convertEulerToQuaternion_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/euler/divideEuler_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/euler/multiplyEuler_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/euler/weightedAverageEuler_node.cpp"
	# Interpolate
	"${CMAKE_CURRENT_SOURCE_DIR}/interpolate/lerp_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/interpolate/lerpAngle_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/interpolate/lerpMatrix_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/interpolate/lerpVector_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/interpolate/slerpQuaternion_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/interpolate/smoothstep_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/interpolate/smoothstepAngle_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/interpolate/smoothstepMatrix_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/interpolate/smoothstepVector_node.cpp"
	# Logic
	"${CMAKE_CURRENT_SOURCE_DIR}/logic/andBool_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/logic/andInt_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/logic/orBool_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/logic/orInt_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/logic/notBool_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/logic/notInt_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/logic/xorBool_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/logic/xorInt_node.cpp"
	# Matrix
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/averageMatrix_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/composeMatrix_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/decomposeMatrix_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/extractBasisXFromMatrix_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/extractBasisYFromMatrix_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/extractBasisZFromMatrix_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/extractEulerFromMatrix_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/extractQuaternionFromMatrix_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/extractRotationMatrix_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/extractScaleFromMatrix_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/extractScaleMatrix_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/extractTranslationFromMatrix_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/extractTranslationMatrix_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/multiplyMatrix_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/multiplyMatrixArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/postMultiplyMatrixByAxisXAngle_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/postMultiplyMatrixByAxisYAngle_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/postMultiplyMatrixByAxisZAngle_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/postMultiplyMatrixByEuler_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/postMultiplyMatrixByQuaternion_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/postMultiplyMatrixByScale_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/postMultiplyMatrixByTranslation_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/preMultiplyMatrixByAxisXAngle_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/preMultiplyMatrixByAxisYAngle_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/preMultiplyMatrixByAxisZAngle_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/preMultiplyMatrixByEuler_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/preMultiplyMatrixByQuaternion_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/preMultiplyMatrixByScale_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/preMultiplyMatrixByTranslation_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix/weightedAverageMatrix_node.cpp"
	# Power
	"${CMAKE_CURRENT_SOURCE_DIR}/power/power_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/power/squareRoot_node.cpp"
	# Quaternion
	"${CMAKE_CURRENT_SOURCE_DIR}/quaternion/averageQuaternion_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/quaternion/convertQuaternionToEuler_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/quaternion/convertQuaternionToMatrix_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/quaternion/multiplyQuaternion_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/quaternion/multiplyQuaternionArray_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/quaternion/weightedAverageQuaternion_node.cpp"
	# Range
	"${CMAKE_CURRENT_SOURCE_DIR}/range/clamp_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/range/clampAngle_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/range/clampInt_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/range/remap_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/range/remapAngle_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/range/remapInt_node.cpp"
	# Round
	"${CMAKE_CURRENT_SOURCE_DIR}/round/ceil_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/round/ceilAngle_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/round/floor_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/round/floorAngle_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/round/round_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/round/roundAngle_node.cpp"
	# Trigonometry
	"${CMAKE_CURRENT_SOURCE_DIR}/trigonometry/acos_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/trigonometry/asin_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/trigonometry/atan_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/trigonometry/atan2_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/trigonometry/cartesianToPolar_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/trigonometry/cos_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/trigonometry/polarToCartesian_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/trigonometry/sin_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/trigonometry/tan_node.cpp"
	# Vector
	"${CMAKE_CURRENT_SOURCE_DIR}/vector/addVector_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector/angleBetweenVectors_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector/averageVector_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector/closestPointOnLine_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector/closestPointOnPlane_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector/crossProduct_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector/divideVector_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector/dotProduct_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector/limitVector_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector/multiplyPointByMatrix_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector/multiplyVector_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector/multiplyVectorByMatrix_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector/negateVector_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector/normalizeVector_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector/reflectVector_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector/rotateVectorByAxisAngle_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector/rotateVectorByEuler_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector/rotateVectorByQuaternion_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector/signedAngleBetweenVectors_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector/subtractVector_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector/sumVector_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector/vectorLength_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector/vectorLengthSquared_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector/vectorPlaneProjection_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector/vectorProjection_node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector/weightedAverageVector_node.cpp"
	# Plugin
	"${CMAKE_CURRENT_SOURCE_DIR}/plugin.cpp")

set(INCLUDE_DIRS
	"${CMAKE_CURRENT_SOURCE_DIR}/arithmetic"
	"${CMAKE_CURRENT_SOURCE_DIR}/condition"
	"${CMAKE_CURRENT_SOURCE_DIR}/distance"
	"${CMAKE_CURRENT_SOURCE_DIR}/euler"
	"${CMAKE_CURRENT_SOURCE_DIR}/interpolate"
	"${CMAKE_CURRENT_SOURCE_DIR}/logic"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix"
	"${CMAKE_CURRENT_SOURCE_DIR}/power"
	"${CMAKE_CURRENT_SOURCE_DIR}/quaternion"
	"${CMAKE_CURRENT_SOURCE_DIR}/range"
	"${CMAKE_CURRENT_SOURCE_DIR}/round"
	"${CMAKE_CURRENT_SOURCE_DIR}/statistics"
	"${CMAKE_CURRENT_SOURCE_DIR}/trigonometry"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector")

# Target
add_library(${TARGET_NAME} SHARED ${CPP_FILES})
# Target - Link utils_static
target_link_libraries(${TARGET_NAME} PRIVATE ${TARGET_PREFIX}utils_static ${TARGET_PREFIX}data_static) # utils_static dependencies (ie. Maya) are transitive
# Target - Set Properties (INTERFACE settings such as INTERFACE_COMPILE_DEFINITIONS are inherited from Maya)
set_target_properties(${TARGET_NAME} PROPERTIES
	INCLUDE_DIRECTORIES	"${INCLUDE_DIRS}" # Same as: target_include_directories(${TARGET_NAME} PRIVATE ${INCLUDE_DIRS})
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