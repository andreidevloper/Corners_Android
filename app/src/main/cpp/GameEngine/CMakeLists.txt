# Set project name
project (GameEngine)

# Define target name
set (TARGET_NAME GameEngine)

# Set minimum version
cmake_minimum_required (VERSION 3.0.2)
if (COMMAND cmake_policy)
    cmake_policy (SET CMP0003 NEW)
    if (CMAKE_VERSION VERSION_GREATER 2.8.12 OR CMAKE_VERSION VERSION_EQUAL 2.8.12)
        # INTERFACE_LINK_LIBRARIES defines the link interface
        cmake_policy (SET CMP0022 NEW)
    endif ()
    if (CMAKE_VERSION VERSION_GREATER 3.0.0 OR CMAKE_VERSION VERSION_EQUAL 3.0.0)
        # Disallow use of the LOCATION target property - therefore we set to OLD as we still need it
        cmake_policy (SET CMP0026 OLD)
        # MACOSX_RPATH is enabled by default
        cmake_policy (SET CMP0042 NEW)
    endif ()
endif ()

if (WIN32)
	set (CMAKE_DEBUG_POSTFIX "_d")
	set (CMAKE_RELWITHDEBINFO_POSTFIX "_rwd")
endif ()

if(ANDROID)
    if (NOT CMAKEROOT)
       set (CMAKEROOT "${PROJECT_SOURCE_DIR}/../../../../")
    endif ()

    set (PROJECTROOT ${CMAKEROOT}src)

    set (ENV{URHO3D_HOME} ${PROJECTROOT}/main/cpp/Urho3D_Framework_1_7/Android_Build_${ANDROID_ABI}_${CMAKE_BUILD_TYPE})
    set (URHO3D_HOME ${PROJECTROOT}/main/cpp/Urho3D_Framework_1_7/Android_Build_${ANDROID_ABI}_${CMAKE_BUILD_TYPE})

    set (CMAKE_MODULE_PATH ${PROJECTROOT}/main/cpp/Urho3D_Framework_1_7/CMake/Modules)
else()
    set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)
endif()

# Include Urho3D Cmake common module
include (UrhoCommon)

# Define files
file(GLOB_RECURSE ENGINE_INCLUDE_FILES *.h)
file(GLOB_RECURSE ENGINE_SOURCE_FILES *.cpp)

# Add directory as include path
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}")

# Define source files
define_source_files (EXTRA_H_FILES ${COMMON_SAMPLE_H_FILES} GLOB_CPP_PATTERNS *.c* GLOB_H_PATTERNS *.h* RECURSE GROUP )
# Setup target with resource copying
setup_library ()
