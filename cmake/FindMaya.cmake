# Original work Copyright 2017 Chad Vernon
# Modified work Copyright 2018 Mitchell Simmons
# You may use, distribute, or modify this code under the terms of the MIT license.

# ----------------------------------------------------------------------------------------------------------------------------
# This module will define the following package which can be used with find_package():
#	MAYA
# ----------------------------------------------------------------------------------------------------------------------------
# This module will define the following IMPORTED target which can be used with target_link_libraries():
#	Maya
# ----------------------------------------------------------------------------------------------------------------------------
# This module will set the following external cache variables (global, can be changed by user):
#	MAYA_VERSION
#	MAYA_LOCATION_BASE
# ----------------------------------------------------------------------------------------------------------------------------
# This module will set the following internal/advanced cache variables (global, internally overriden each run):
#	MAYA_LOCATION_FULL			(forced)
#	MAYA_LIBRARIES				(forced)
#	MAYA_INCLUDE_DIR			(unset -> cached -> advanced)
#	MAYA_LIBRARY_DIR			(unset -> cached -> advanced)
# ----------------------------------------------------------------------------------------------------------------------------

# OS Specific environment setup
set(MAYA_COMPILE_DEFINITIONS "REQUIRE_IOSTREAM;_BOOL")

# Set a default Maya version
set(MAYA_VERSION 2019 CACHE STRING "Maya version")

# Unset variables which may have changed since the previous run (eg. if the MAYA_VERSION changes, this should affect the paths)
# Commands such as find_path() and find_library() will only run if the variables they operate on are clear (ie. they set CACHE variables by default which persist between runs)
unset(MAYA_INCLUDE_DIR CACHE)
unset(MAYA_LIBRARY_DIR CACHE)

if(WIN32)
    set(MAYA_COMPILE_DEFINITIONS "${MAYA_COMPILE_DEFINITIONS};NT_PLUGIN")
    set(MAYA_PLUGIN_EXTENSION ".mll")

	set(MAYA_LOCATION_BASE "C:/Program Files/Autodesk/" CACHE PATH "Autodesk installation directory")
	set(MAYA_LOCATION_FULL "${MAYA_LOCATION_BASE}/Maya${MAYA_VERSION}" CACHE INTERNAL "Maya executable location") # INTERNAL implies FORCE

	find_path(MAYA_INCLUDE_DIR maya/MFn.h PATHS ${MAYA_LOCATION_FULL} PATH_SUFFIXES include/)
	find_path(MAYA_LIBRARY_DIR OpenMaya.lib PATHS ${MAYA_LOCATION_FULL} PATH_SUFFIXES lib/)
elseif(APPLE)
    set(MAYA_COMPILE_DEFINITIONS "${MAYA_COMPILE_DEFINITIONS};OSMac_")
    set(MAYA_PLUGIN_EXTENSION ".bundle")

	set(MAYA_LOCATION_BASE "/Applications/Autodesk/" CACHE PATH "Autodesk installation directory")
	set(MAYA_LOCATION_FULL "${MAYA_LOCATION_BASE}/maya${MAYA_VERSION}" CACHE INTERNAL "Maya executable location")

	find_path(MAYA_INCLUDE_DIR maya/MFn.h PATHS ${MAYA_LOCATION_FULL} PATH_SUFFIXES include/)
	find_path(MAYA_LIBRARY_DIR libOpenMaya.dylib PATHS ${MAYA_LOCATION_FULL} PATH_SUFFIXES Maya.app/Contents/MacOS/)
else()
    set(MAYA_COMPILE_DEFINITIONS "${MAYA_COMPILE_DEFINITIONS};LINUX")
    set(MAYA_PLUGIN_EXTENSION ".so")

	set(MAYA_LOCATION_BASE "/usr/autodesk/" CACHE PATH "Autodesk installation directory")
	set(MAYA_LOCATION_FULL "${MAYA_LOCATION_BASE}/maya${MAYA_VERSION}" CACHE INTERNAL "Maya executable location")

	find_path(MAYA_INCLUDE_DIR maya/MFn.h PATHS ${MAYA_LOCATION_FULL} PATH_SUFFIXES include/)
	find_path(MAYA_LIBRARY_DIR libOpenMaya.so PATHS ${MAYA_LOCATION_FULL} PATH_SUFFIXES lib/)
endif()

# These variables are external by default, advanced will hide them from any cmake GUIs
mark_as_advanced(MAYA_INCLUDE_DIR)
mark_as_advanced(MAYA_LIBRARY_DIR)

# Find Maya libraries
set(MAYA_LIBRARIES CACHE INTERNAL "Maya main library files")
set(_MAYA_LIBRARIES Foundation MetaData OpenMaya OpenMayaAnim OpenMayaFX OpenMayaRender OpenMayaUI)
foreach(MAYA_LIB ${_MAYA_LIBRARIES})
	# Again, we must ensure these are unset or they will not update
	unset(MAYA_${MAYA_LIB}_LIBRARY CACHE)
    find_library(MAYA_${MAYA_LIB}_LIBRARY NAMES ${MAYA_LIB} PATHS ${MAYA_LIBRARY_DIR} NO_DEFAULT_PATH)
    if (MAYA_${MAYA_LIB}_LIBRARY)
		# Set variable containing current and new paths
        set(MAYA_LIBRARIES ${MAYA_LIBRARIES} ${MAYA_${MAYA_LIB}_LIBRARY} CACHE INTERNAL "Maya main library files")
		mark_as_advanced(MAYA_${MAYA_LIB}_LIBRARY)
    endif()
endforeach()

# Defines a package named MAYA which can be used to load the variables from the current find module into the calling scope of the command find_package(MAYA)
# The variable MAYA_FOUND will be set to TRUE if all variables given to the below command are valid
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MAYA DEFAULT_MSG MAYA_LOCATION_FULL MAYA_INCLUDE_DIR MAYA_LIBRARIES)

# Provide users access to a target which can be consumed when target_link_libraries(consumer, Maya) is called
# An import library is one that makes use of already built libraries on disk and treats them as a target
# INTERFACE properties only apply to consuming/depending targets, not the library itself
# CMake recommends namespacing import targets as it will provide appropriate diagnostic messages if it does not exist (seems like the above command would have already failed in this case?)
if(MAYA_FOUND AND NOT TARGET Maya)
	add_library(Maya STATIC IMPORTED)
	set_target_properties(Maya PROPERTIES
		IMPORTED_LOCATION "${MAYA_OpenMaya_LIBRARY}" # This is not the directory path, this has to be a path to the main library file of our IMPORT target
		INTERFACE_COMPILE_DEFINITIONS "${MAYA_COMPILE_DEFINITIONS}"
		INTERFACE_INCLUDE_DIRECTORIES "${MAYA_INCLUDE_DIR}"
		INTERFACE_LINK_LIBRARIES "${MAYA_LIBRARIES}") # Specify all other libraries which should be consumed by dependent targets
endif()