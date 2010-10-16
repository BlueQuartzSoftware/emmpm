project(emmpm)
cmake_minimum_required(VERSION 2.6)

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

SET (emmpm_PROJECT_DIR ${emmpm_SOURCE_DIR})
SET (PROJECT_RESOURCES_DIR ${emmpm_SOURCE_DIR}/Resources)
SET (emmpm_CMAKE_DIR  ${PROJECT_RESOURCES_DIR}/CMake)
SET (PROJECT_CODE_DIR ${PROJECT_SOURCE_DIR}/Source)
SET (OSX_TOOLS_DIR ${PROJECT_RESOURCES_DIR}/OSX_Tools)
SET (PROJECT_PREFIX "EMMPM" CACHE STRING "The 3 Letter Prefix to be used for Preprocessor definitions")
SET (EXE_DEBUG_EXTENSION "_debug")

INCLUDE (${PROJECT_RESOURCES_DIR}/CMake/CMakeMacros.cmake )
INCLUDE (${PROJECT_RESOURCES_DIR}/CMake/ConfigureChecks.cmake)

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

# Include some directories for the compiler
include_directories(${PROJECT_BINARY_DIR})
include_directories(${emmpm_SOURCE_DIR}/Source)

# --------------------------------------------------------------------
# Generate a Header file with Compile Version variables
# --------------------------------------------------------------------
GenerateVersionString( ${PROJECT_NAME} "${emmpm_BINARY_DIR}/${PROJECT_PREFIX}/Common/${PROJECT_PREFIX}Version.h" "${PROJECT_PREFIX}" )

# --------------------------------------------------------------------
# Generate our header files that contain the configuration information
# and things like standard int typedefs.
# --------------------------------------------------------------------
configure_file(${PROJECT_RESOURCES_DIR}/CMake/Configuration.h.in
               ${emmpm_BINARY_DIR}/${PROJECT_PREFIX}/Common/${PROJECT_PREFIX}Configuration.h )
configure_file(${PROJECT_RESOURCES_DIR}/CMake/PrimitiveTypes.h.in
               ${emmpm_BINARY_DIR}/${PROJECT_PREFIX}/Common/${PROJECT_PREFIX}Types.h )

# Prepend our own "FindTiff" code in front of what CMake itself provides
set(CMAKE_MODULE_PATH ${PROJECT_RESOURCES_DIR}/CMake ${CMAKE_MODULE_PATH} )
FIND_PACKAGE(Tiff)
IF (NOT TIFF_FOUND)
      MESSAGE (FATAL_ERROR " Tiff Library was NOT Found and is needed.")
ENDIF (NOT TIFF_FOUND)
INCLUDE_DIRECTORIES(${TIFF_INCLUDE_DIRS})
    
    
set(emmpm_SRCS
    ${PROJECT_CODE_DIR}/allocate.c
    ${PROJECT_CODE_DIR}/em.c
    ${PROJECT_CODE_DIR}/entropy.c
    ${PROJECT_CODE_DIR}/mpm.c
    ${PROJECT_CODE_DIR}/random.c
    ${PROJECT_CODE_DIR}/EMTiffIO.c
	${PROJECT_CODE_DIR}/EMTime.c
)
set (emmpm_HDRS
    ${PROJECT_CODE_DIR}/allocate.h
    ${PROJECT_CODE_DIR}/random.h
    ${PROJECT_CODE_DIR}/EMTiffIO.h
    ${PROJECT_CODE_DIR}/EMTime.h
	${PROJECT_CODE_DIR}/MSVCDefines.h
)
# --------------------------------------------------------------------
# This is a macro to help the Visual Studio Project be a bit more organized
IDE_SOURCE_PROPERTIES( Code "${emmpm_HDRS}" "${emmpm_SRCS}")


# --------------------------------------------------------------------
# Create an Executable using the sources listed
add_executable(emmpm ${emmpm_SRCS} ${emmpm_HDRS} )

# --------------------------------------------------------------------
# Add a link stage to link the executable to the tiff library
if (UNIX AND NOT APPLE)
set (MATH_LIBRARY "m")
endif()

target_link_libraries(emmpm ${TIFF_LIBRARY} ${MATH_LIBRARY})


add_subdirectory(${emmpm_SOURCE_DIR}/Example ${emmpm_BINARY_DIR}/Example)

