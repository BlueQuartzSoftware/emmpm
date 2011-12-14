#///////////////////////////////////////////////////////////////////////////////
#
#  Copyright (c) 2011, Michael A. Jackson. BlueQuartz Software
#  All rights reserved.
#  BSD License: http://www.opensource.org/licenses/bsd-license.html
#
#///////////////////////////////////////////////////////////////////////////////

project(emmpm)
cmake_minimum_required(VERSION 2.8)

# ---------- Setup output Directories -------------------------
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY
  ${PROJECT_BINARY_DIR}/Bin
  CACHE PATH
  "Single Directory for all Libraries"
  )

# --------- Setup the Executable output Directory -------------
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY
  ${PROJECT_BINARY_DIR}/Bin
  CACHE PATH
  "Single Directory for all Executables."
  )

# --------- Setup the Executable output Directory -------------
SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY
  ${PROJECT_BINARY_DIR}/Bin
  CACHE PATH
  "Single Directory for all static libraries."
  )

if (NOT DEFINED CMP_SOURCE_DIR)
set(CMP_SOURCE_DIR ${emmpm_SOURCE_DIR}/Support/cmp)
endif()


SET (PROJECT_CODE_DIR ${PROJECT_SOURCE_DIR}/Code)
SET (PROJECT_RESOURCES_DIR ${PROJECT_SOURCE_DIR}/Resources)
SET (PROJECT_PREFIX "EMMPM" CACHE STRING "The 3 Letter Prefix to be used for Preprocessor definitions")
SET (EXE_DEBUG_EXTENSION "_debug")
SET (PROJECT_INSTALL_HEADERS "1")
SET (PROJECT_INSTALL_EXECUTABLES "1")

#-------------------------------------------------------------------------------
# Include the cmp project to perform all the basic configuration tests for the 
# project.
#------------------------------------------------------------------------------- 
set (CMP_HEADER_DIR ${emmpm_BINARY_DIR}/emmpm/common)
set (CMP_CONFIGURATION_FILE_NAME "CMPConfiguration.h")
set (CMP_TYPES_FILE_NAME "EMMPMTypes.h")
set (CMP_VERSION_HEADER_FILE_NAME "EMMPMVersion.h")
set (CMP_EXTRA_CONFIGURATION_FILE "EMMPMConfiguration.h")
set (CMP_PROJECT_NAMESPACE "EMMPM")
set (CMP_PROJECT_NAME "EMMPM")
SET (CMP_INSTALL_FILES "1")
set (CMP_ENABLE_PLUGINS "0")
INCLUDE (${CMP_SOURCE_DIR}/cmpProject.cmake)
               
# --------------------------------------------------------------------
# Generate our ReadMe and License Files
configure_file(${PROJECT_RESOURCES_DIR}/emmpm/License.txt.in
                ${PROJECT_BINARY_DIR}/License.txt )
configure_file(${PROJECT_RESOURCES_DIR}/emmpm/ReadMe.txt.in
                ${PROJECT_BINARY_DIR}/ReadMe.txt )
               
# Build shared libraries
OPTION (BUILD_SHARED_LIBS "Build Shared Libraries" OFF)
set (EMMPM_BUILT_AS_DYNAMIC_LIB )
SET (LIB_TYPE STATIC)
IF (BUILD_SHARED_LIBS)
  SET (LIB_TYPE SHARED)
  SET (EMMPM_BUILT_AS_DYNAMIC_LIB 1)
    IF (WIN32)
    ADD_DEFINITIONS("-DEMMPM_BUILT_AS_DYNAMIC_LIB")
  ENDIF (WIN32)
ENDIF (BUILD_SHARED_LIBS)

# --------------------------------------------------------------------
# Possibly use a different Random Number Generator
set(USE_MERSENNE_TWISTER)
option(EMMPM_USE_MT_RNG "Use the Mersenne Twister Random Number Generator" ON)
if (EMMPM_USE_MT_RNG)
  set(USE_MERSENNE_TWISTER 1)
endif()
# --------------------------------------------------------------------
# Now configure this extra file with some additional definitions
configure_file(${PROJECT_RESOURCES_DIR}/emmpm/EMMPMConfiguration.h.in
               ${CMP_HEADER_DIR}/${CMP_EXTRA_CONFIGURATION_FILE})
               
# --------------------------------------------------------------------
# Add in some compiler definitions
# --------------------------------------------------------------------
IF ( CMAKE_BUILD_TYPE MATCHES Debug )
  ADD_DEFINITIONS(-DDEBUG)
  IF (NOT WIN32)
     #Doing this with NMake files will overwhelm the output
    ADD_DEFINITIONS(-Wall)
  ENDIF()
ENDIF ( CMAKE_BUILD_TYPE MATCHES Debug )

# --------------------------------------------------------------------
# If was are using GCC, make the compiler messages on a single line
IF(CMAKE_COMPILER_IS_GNUCC)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCC)
IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

IF (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
ENDIF()

# --------------------------------------------------------------------
# Add in the math library on non-Apple and non-Windows platforms
if (UNIX AND NOT APPLE)
    set (MATH_LIBRARY "m")
endif()

# Prepend our own "FindTiff" code in front of what CMake itself provides
set (EMMPM_TIFF_SUPPORT)
FIND_PACKAGE(Tiff)
IF (NOT TIFF_FOUND)
      MESSAGE (FATAL_ERROR " Tiff Library was NOT Found and is needed.")
ENDIF (NOT TIFF_FOUND)
set (EMMPM_TIFF_SUPPORT "1")
INCLUDE_DIRECTORIES(${TIFF_INCLUDE_DIRS})

# Include some directories for the compiler
include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_CODE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/Utilities)

# Should we build the HTML documentation
option(emmpm_BUILD_API_DOCS "Use Doxygen to create the HTML based API documentation" OFF)
if(emmpm_BUILD_API_DOCS)
  FIND_PACKAGE(Doxygen)
  if (NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen is needed to build the documentation. Please install it correctly")
  endif()
  configure_file(${PROJECT_RESOURCES_DIR}/emmpm.doxyfile.in 
                 ${PROJECT_BINARY_DIR}/Doxyfile  @ONLY IMMEDIATE)
  add_custom_target(docs COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile)
  install(DIRECTORY ${PROJECT_BINARY_DIR}/Documentation/html/
          DESTINATION Documentation 
          COMPONENT Applications)
endif()


add_subdirectory(${PROJECT_CODE_DIR}/emmpm ${emmpm_BINARY_DIR}/EMMPMLib)
add_subdirectory(${PROJECT_CODE_DIR}/apps ${emmpm_BINARY_DIR}/apps)

include(${PROJECT_RESOURCES_DIR}/CPack/PackageProject.cmake)

