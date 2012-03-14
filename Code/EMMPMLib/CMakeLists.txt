#--////////////////////////////////////////////////////////////////////////////
#-- Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
#-- All rights reserved.
#-- BSD License: http://www.opensource.org/licenses/bsd-license.html
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--////////////////////////////////////////////////////////////////////////////
project (EMMPMLib)
cmake_minimum_required(VERSION 2.8.7)

SET (EMMPMLib_BUILT_AS_DYNAMIC_LIB)
IF (BUILD_SHARED_LIBS)
    SET (EMMPMLib_BUILT_AS_DYNAMIC_LIB 1)
    IF (WIN32)
        ADD_DEFINITIONS("-DEMMPMLib_BUILT_AS_DYNAMIC_LIB")
    ENDIF (WIN32)
ENDIF (BUILD_SHARED_LIBS)



option(EMMPM_DISABLE_MSVC_WARNINGS "Disable some MSVC Compiler warnings" OFF)
IF (EMMPM_DISABLE_MSVC_WARNINGS)
    add_definitions(-DEMMPM_DISABLE_MSVC_WARNINGS)
endif()

SET (PROJECT_PREFIX "EMMPM" CACHE STRING "The Prefix to be used for Preprocessor definitions")
set (PROJECT_INSTALL_HEADERS "0")
#-------------------------------------------------------------------------------
# Include the cmp project to perform all the basic configuration tests for the 
# project.
#------------------------------------------------------------------------------- 
set (CMP_HEADER_DIR ${EMMPMLib_BINARY_DIR})
set (CMP_CONFIGURATION_FILE_NAME "EMMPMLibConfiguration.h")
set (CMP_TYPES_FILE_NAME "EMMPMLibTypes.h")
set (CMP_VERSION_HEADER_FILE_NAME "EMMPMLibVersion.h")
set (CMP_TOP_HEADER_FILE "EMMPMLib.h")
set (CMP_PROJECT_NAMESPACE "${PROJECT_NAME}")
set (CMP_PROJECT_NAME "${PROJECT_NAME}")
set (CMP_ENABLE_PLUGINS "0")
set (CMP_LIB_SEARCH_DIRS "")
set (CMP_GENERATE_VERSION_STRING 1)


INCLUDE (${CMP_SOURCE_DIR}/cmpProject.cmake)
configure_file(${EMMPMLib_SOURCE_DIR}/EMMPMConfiguration.h.in
               ${EMMPMProj_BINARY_DIR}/EMMPMLib/${CMP_TOP_HEADER_FILE})


set(EMMPMLib_PROJECT_FILES "")
# --------------------------------------------------------------------
Macro(Add_EMMPMLib_Dir VarName Path)
    include(${EMMPMLib_SOURCE_DIR}/${Path}/SourceList.cmake)
    set (EMMPMLib_PROJECT_FILES ${EMMPMLib_PROJECT_FILES}
                                        ${EMMPM_${VarName}_SRCS}
                                        ${EMMPM_${VarName}_HDRS} )
endMacro()

Add_EMMPMLib_Dir(Public public)
Add_EMMPMLib_Dir(Common common)
Add_EMMPMLib_Dir(Tiff tiff)
Add_EMMPMLib_Dir(Private private)

if (EMMPM_USE_MT_RNG)
    Add_EMMPMLib_Dir(mt mt)
endif()
# --------------------------------------------------------------------
# Add in the configured headers

set (EMMPMLib_CONFIGURED_HDRS
    "${CMP_HEADER_DIR}/${CMP_CONFIGURATION_FILE_NAME}" 
    "${CMP_HEADER_DIR}/${CMP_TYPES_FILE_NAME}" 
    "${CMP_HEADER_DIR}/${CMP_VERSION_HEADER_FILE_NAME}"
    "${CMP_HEADER_DIR}/${CMP_EXTRA_CONFIGURATION_FILE}"
)
set (EMMPMLib_CONFIGURED_SRCS
)
cmp_IDE_SOURCE_PROPERTIES("emmpm/common" "${EMMPMLib_CONFIGURED_HDRS}" "${EMMPMLib_CONFIGURED_SRCS}" "${PROJECT_INSTALL_HEADERS}")

set (EMMPMLib_PROJECT_FILES ${EMMPMLib_PROJECT_FILES} ${EMMPMLib_CONFIGURED_HDRS} )

add_library(EMMPMLib ${LIB_TYPE} ${EMMPMLib_PROJECT_FILES})
target_link_libraries(EMMPMLib ${TIFF_LIBRARY} ${MATH_LIBRARY})
LibraryProperties(EMMPMLib ${EXE_DEBUG_EXTENSION} )

set(install_dir "tools")
set(lib_install_dir "lib")
if (WIN32)
    set (install_dir ".")
    set(lib_install_dir "")
endif()


if (BUILD_SHARED_LIBS AND NOT APPLE)
    INSTALL(TARGETS ${PROJECT_NAME}  
        COMPONENT Applications
        RUNTIME DESTINATION ${install_dir}
        LIBRARY DESTINATION ${lib_install_dir}
        ARCHIVE DESTINATION ${lib_install_dir}       
        BUNDLE DESTINATION "."
    ) 
endif() 
