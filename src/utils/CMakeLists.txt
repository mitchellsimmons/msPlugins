project(utils VERSION 1.0.0)
set(TARGET_NAME "${TARGET_PREFIX}${PROJECT_NAME}_static")

# Files
set(CPP_FILES		
	"${CMAKE_CURRENT_SOURCE_DIR}/color_utils.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/command_utils.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/data_utils.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/math_utils.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix_utils.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/name_utils.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/node_utils.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/plugin_utils.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/quaternion_utils.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/spline_utils.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector_utils.cpp")

set(HEADER_FILES	
	"${CMAKE_CURRENT_SOURCE_DIR}/color_utils.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/command_utils.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/data_utils.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/macros.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/math_utils.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/matrix_utils.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/name_utils.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/node_utils.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/plugin_utils.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/quaternion_utils.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/spline_utils.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector_utils.h")

# Target
add_library("${TARGET_NAME}" STATIC ${CPP_FILES})
# Target - Link External
# Note, PUBLIC is default here (dependencies will be transitive)
unset(SeExpr2_LIB CACHE)
find_library(SeExpr2_LIB NAMES "SeExpr2" PATHS ${SeExpr2_LIBRARY_DIRS} NO_DEFAULT_PATH)
target_link_libraries("${TARGET_NAME}" PUBLIC Maya ${SeExpr2_LIB}) 
# Target - Link Include Directories
# Ensure consuming targets have access when target_link_libraries(consumer, utils) is called
# Note, the Maya target links automatically, as it was created with an interface (ie. the interface is inherited by the consumer)
# Below we are using INTERFACE because we want consuming targets to have access to header files (they will already be available internally)
# We have traversed up a directory so that includes can occur relative to "utils/"
target_include_directories("${TARGET_NAME}" INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/..")
# Below we are using PUBLIC to populate both INTERFACE_INCLUDE_DIRECTORIES and INCLUDE_DIRECTORIES
# We want these headers to be available internally and to be passed transitively to consuming targets
target_include_directories("${TARGET_NAME}" PUBLIC "${SeExpr2_INCLUDE_DIRS};${Eigen_INCLUDE_DIRS}")
# Target - Set Properties
# Note, INTERFACE settings such as INTERFACE_COMPILE_DEFINITIONS are inherited from Maya
set_target_properties("${TARGET_NAME}" PROPERTIES
	# Lets us intall the headers in a single call below
	PUBLIC_HEADER "${HEADER_FILES}"
	# VS - Organize project into a folder
	FOLDER "archive")

# Install - Targets
if(INSTALL_INTERNAL_TARGETS)
	install(TARGETS "${TARGET_NAME}"
		ARCHIVE DESTINATION "${LIB_INSTALL_DIR}"
		PUBLIC_HEADER DESTINATION "${INCLUDE_INSTALL_DIR}/${PROJECT_NAME}")
endif()