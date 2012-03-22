#///////////////////////////////////////////////////////////////////////////////
#
#  Copyright (c) 2011, Michael A. Jackson. BlueQuartz Software
#  All rights reserved.
#  BSD License: http://www.opensource.org/licenses/bsd-license.html
#
#///////////////////////////////////////////////////////////////////////////////

project(EMMPMProj)
cmake_minimum_required(VERSION 2.8.7)

include (CMakeParseArguments)

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




SET (PROJECT_CODE_DIR ${PROJECT_SOURCE_DIR}/Code)
SET (PROJECT_RESOURCES_DIR ${PROJECT_SOURCE_DIR}/Resources)
#SET (PROJECT_PREFIX "EMMPM" CACHE STRING "The 3 Letter Prefix to be used for Preprocessor definitions")
SET (EXE_DEBUG_EXTENSION "_debug")
SET (PROJECT_INSTALL_HEADERS "0")
SET (PROJECT_INSTALL_EXECUTABLES "1")

if (NOT DEFINED CMP_SOURCE_DIR)
set(CMP_SOURCE_DIR ${EMMPMProj_SOURCE_DIR}/Support/cmp)
endif()
INCLUDE (${CMP_SOURCE_DIR}/cmpCMakeMacros.cmake )
set (CMP_ENABLE_PLUGINS "0")
set (CMP_PLUGIN_LIST_FILE ${PROJECT_BINARY_DIR}/EMMPM_PluginList.txt)
file(WRITE ${CMP_PLUGIN_LIST_FILE} "")
set (CMP_PLUGIN_SEARCHDIR_FILE ${PROJECT_BINARY_DIR}/libsearchdirs.txt)
file(WRITE ${CMP_PLUGIN_SEARCHDIR_FILE} "${PROJECT_BINARY_DIR}/Bin/plugins;")
file(APPEND ${CMP_PLUGIN_SEARCHDIR_FILE} "${PROJECT_BINARY_DIR}/Bin;")
set (CMP_LIB_SEARCH_DIRS "")
set(CMP_MODULES_SOURCE_DIR ${CMP_SOURCE_DIR}/Modules CACHE INTERNAL "")

# --------------------------------------------------------------------
# In order to increment the version of DREAM3D simply adjust these variables.
# we are using a 3 part Version scheme commonly found in software development. The
# example would be Version 2.1.6543 which translates to Version [major].[minor].[patch].
# The Major version is set here in the CMake file. We will use Git 
# to figure out the "patch" leve of the version by using "git describe" which will
# calculate how many commits since the last "tag" was applied to the repository.
# this basically gives us an always incrementing number for the patch level
# and we can set the Major and Minor versions as we see fit. 
set(CMP_VERSION_MAJOR "v2")


# --------------------------------------------------------------------
# Over ride CMake's built in module directory by prepending cmp's module
# directory first
SET (CMAKE_MODULE_PATH ${CMP_MODULES_SOURCE_DIR} ${CMAKE_MODULE_PATH})

# Build shared libraries
OPTION (BUILD_SHARED_LIBS "Build Shared Libraries" OFF)
SET (LIB_TYPE STATIC)
IF (BUILD_SHARED_LIBS)
    SET (LIB_TYPE SHARED)
    list(APPEND CMP_LIB_SEARCH_DIRS  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )
    if (MSVC)
        list(APPEND CMP_LIB_SEARCH_DIRS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug
                                    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release )
    endif()
ENDIF (BUILD_SHARED_LIBS)

# ---------- Find Boost Headers/Libraries -----------------------
#SET (Boost_FIND_REQUIRED FALSE)
SET (Boost_DEBUG FALSE)
set (Boost_USE_MULTITHREADED TRUE)
set (Boost_USE_STATIC_LIBS TRUE)
SET (Boost_ADDITIONAL_VERSIONS "1.47.0" "1.46.0" "1.44.0" "1.44" "1.41" "1.41.0" "1.39" "1.39.0")
set (Boost_FIND_COMPONENTS "")
set (Boost_NO_BOOST_CMAKE 1)
FIND_PACKAGE(Boost)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

# --------------------------------------------------------------------
# Possibly use a different Random Number Generator
set(USE_MERSENNE_TWISTER)
option(EMMPMLib_USE_MT_RNG "Use the Mersenne Twister Random Number Generator" ON)
if (EMMPMLib_USE_MT_RNG)
  set(USE_MERSENNE_TWISTER 1)
endif()


# --------------------------------------------------------------------
# Should we use Intel Threading Building Blocks
# --------------------------------------------------------------------
set (EMMPMLib_USE_PARALLEL_ALGORITHMS "")
option(EMMPMLib_USE_MULTITHREADED_ALGOS "Use MultiThreaded Algorithms where possible" OFF)
if (EMMPMLib_USE_MULTITHREADED_ALGOS)
    find_package(TBB)
    if (TBB_FOUND)
        include_directories(${TBB_INCLUDE_DIRS} )
        CMP_COPY_DEPENDENT_LIBRARIES("tbb;tbb_malloc")
        # Append the locations of the TBB libraries to our Library Search Paths
        list(APPEND CMP_LIB_SEARCH_DIRS ${TBB_BINARY_DIR} ${TBBG_LIBRARY_DIR} )
        # Create an install rule to copy the dependent shared library over to the installed location
        CMP_LIBRARIES_INSTALL_RULES("tbb;tbb_malloc" ".")
    else()
        message(FATAL_ERROR "The Intel Threading Building Blocks library is needed to enable the multithreaded algorithms. Please make sure it is installed properly")
    endif()
    set (EMMPMLib_USE_PARALLEL_ALGORITHMS "1")
endif()

# Prepend our own "FindTiff" code in front of what CMake itself provides
set (EMMPMLib_TIFF_SUPPORT)
FIND_PACKAGE(Tiff)
IF (NOT TIFF_FOUND)
      MESSAGE (FATAL_ERROR " Tiff Library was NOT Found and is needed.")
ENDIF (NOT TIFF_FOUND)
set (EMMPMLib_TIFF_SUPPORT "1")
INCLUDE_DIRECTORIES(${TIFF_INCLUDE_DIRS})
# Copy DLL libraries into the correct build location on MSVC platforms
if (HAVE_TIFF_DLL)
CMP_COPY_DEPENDENT_LIBRARIES ("tiff")
CMP_LIBRARIES_INSTALL_RULES ("tiff" "." )
endif()
               
# --------------------------------------------------------------------
# Generate our ReadMe and License Files
configure_file(${PROJECT_RESOURCES_DIR}/emmpm/License.txt.in
                ${PROJECT_BINARY_DIR}/License.txt )
configure_file(${PROJECT_RESOURCES_DIR}/emmpm/ReadMe.txt.in
                ${PROJECT_BINARY_DIR}/ReadMe.txt )

               
# --------------------------------------------------------------------
# Add in some compiler definitions
# --------------------------------------------------------------------
IF ( CMAKE_BUILD_TYPE MATCHES Debug )
  ADD_DEFINITIONS(-DDEBUG)
  IF (NOT WIN32)
     #Doing this with NMake files will overwhelm the output
    ADD_DEFINITIONS(-Wall)
    ADD_DEFINITIONS(-DDEBUG)
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



# Include some directories for the compiler
include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_CODE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/Utilities)

# Should we build the HTML documentation
option(EMMPMLib_BUILD_API_DOCS "Use Doxygen to create the HTML based API documentation" OFF)
if(EMMPMLib_BUILD_API_DOCS)
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


add_subdirectory(${PROJECT_CODE_DIR}/EMMPMLib ${EMMPMProj_BINARY_DIR}/EMMPMLib)
add_subdirectory(${PROJECT_CODE_DIR}/apps ${EMMPMProj_BINARY_DIR}/apps)

include(${PROJECT_RESOURCES_DIR}/CPack/PackageProject.cmake)

