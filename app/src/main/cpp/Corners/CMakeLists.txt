# Set project name
project (Corners)

# Define target name
set (TARGET_NAME Corners)

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

message(STATUS "Native shared library will be created for following Android_ABI = ${ANDROID_ABI}")
message(STATUS "CMake binary dir (where shared libs (.so) keep after compilation) = ${CMAKE_BINARY_DIR}")

message(STATUS "--------------------------------------------------")
if(ANDROID)
    message(STATUS "Start set initial variables for directory searching for Android platform")

    # Location of app folder
    if (NOT CMAKEROOT)
       set (CMAKEROOT "${PROJECT_SOURCE_DIR}/../../../../")
       message(STATUS "Current project path = ${PROJECT_SOURCE_DIR}")
       message(STATUS "Android project's app's folder path = ${CMAKEROOT}")
    endif ()

    # Project root
    set (PROJECTROOT ${CMAKEROOT}src)
    message(STATUS "src's folder path = ${PROJECTROOT}")

    # If produced library go to this path. It will be added to pack.
    set (ANDROID_LIBRARY_OUTPUT_PATH ${CMAKEROOT}/src/main/jniLibs/${ANDROID_ABI}/)
    message(STATUS "jniLibs folder path = ${ANDROID_LIBRARY_OUTPUT_PATH}")

    # For c++_shared AS has bug https://issuetracker.google.com/issues/37139640
    # Currently use gnustl_shared STL lib for Urho3D and Contour application
    set (STL_LIBRARY_DIR ${PROJECTROOT}/main/cpp/Urho3D_Framework_1_7/Android_Build_Corners_${ANDROID_ABI}_${CMAKE_BUILD_TYPE}/libs/${ANDROID_ABI})

    message(STATUS "stl_library_dir folder path (for Urho3D Toolchains/Android.cmake module) = ${STL_LIBRARY_DIR}")
    message(STATUS "${ANDROID_STL} STL library will used for ${TARGET_NAME} project")

    set (ENV{URHO3D_HOME} ${PROJECTROOT}/main/cpp/Urho3D_Framework_1_7/Android_Build_${ANDROID_ABI}_${CMAKE_BUILD_TYPE})
    set (URHO3D_HOME ${PROJECTROOT}/main/cpp/Urho3D_Framework_1_7/Android_Build_${ANDROID_ABI}_${CMAKE_BUILD_TYPE})
    message(STATUS "Urho3D framework build's folder path = ${URHO3D_HOME}")

    # Set CMAKE modules/toolchains
    set (CMAKE_MODULE_PATH ${PROJECTROOT}/main/cpp/Urho3D_Framework_1_7/CMake/Modules)
    message(STATUS "Urho3D framework's folder with CMake modules path = ${URHO3D_HOME}")

   # Set compiler can only in module build.app file
   # if(ANDROID_ABI STREQUAL x86)
   #     set(ANDROID_TOOLCHAIN "gcc")
   # else()
   #     set(ANDROID_TOOLCHAIN "clang")
   # endif()
    message(STATUS "${ANDROID_TOOLCHAIN} compiler is used for ${TARGET_NAME} project")
else()
    set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)
endif()

# Include Urho3D Cmake common module
include (UrhoCommon)

# Find Urho3D library
find_package (Urho3D REQUIRED)

message(STATUS "Setting initial variables for directory searching for Android platform is successfully finished")
message(STATUS "--------------------------------------------------")

# Add include directories 
include_directories (${URHO3D_INCLUDE_DIRS})

# Add directory as include path
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../")

# Define source files
define_source_files (EXTRA_H_FILES ${COMMON_SAMPLE_H_FILES} GLOB_CPP_PATTERNS *.c* GLOB_H_PATTERNS *.h* RECURSE GROUP )

# Setup target with resource copying
setup_main_executable ()

#attempt to link
target_link_libraries(Corners Urho3D GameEngine)

# For VISUAL STUDIO generator
if (MSVC)
    set (RESOURCE_DIRS ${CMAKE_SOURCE_DIR}/bin_Corners/CoreData ${CMAKE_SOURCE_DIR}/bin_Corners/Data ${CMAKE_SOURCE_DIR}/bin_Corners/GameData)
    file(GLOB RESOURCE_FILES "${CMAKE_SOURCE_DIR}/bin_Corners/*.*")
    set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/../../bin")
    # Copy Corners's game data folder
    file(COPY ${CMAKE_SOURCE_DIR}/bin_Corners/GameData DESTINATION ${PROJECT_BINARY_DIR}/../../bin)
endif()

if(ANDROID)
    message(STATUS "Staring copy shared libraries: libUrho3D, libCorners  from ${CMAKE_BINARY_DIR}/libs/${ANDROID_NDK_ABI_NAME} to jniLibs for APK packaging")
    add_custom_command (TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/libs/${ANDROID_NDK_ABI_NAME}/libUrho3D.so ${PROJECTROOT}/main/jniLibs/${ANDROID_ABI}/libUrho3D.so)
    add_custom_command (TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/libs/${ANDROID_NDK_ABI_NAME}/libCorners.so ${PROJECTROOT}/main/jniLibs/${ANDROID_ABI}/libCorners.so)
    # STL library copying (For SHARED STL libraries AS has bug https://issuetracker.google.com/issues/37139640) because that
    # For Corners project use libgnustl_static lib, but for Urho3D libgnustl_shared lib
    message(STATUS "Staring copy shared libraries: gnustl_shared (for Urho3D lib) from ${STL_LIBRARY_DIR} to jniLibs for APK packaging")
    add_custom_command (TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${STL_LIBRARY_DIR}/libgnustl_shared.so ${PROJECTROOT}/main/jniLibs/${ANDROID_ABI}/libgnustl_shared.so)
endif()

message(STATUS "Shared libraries successfully are copied into jniLibs")

message(STATUS "--------------------------------------------------")