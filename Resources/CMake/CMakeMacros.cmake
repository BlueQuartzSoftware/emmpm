#///////////////////////////////////////////////////////////////////////////////
#
#  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
#  All rights reserved.
#  BSD License: http://www.opensource.org/licenses/bsd-license.html
#
#///////////////////////////////////////////////////////////////////////////////

#-------------------------------------------------------------------------------
MACRO (IDE_GENERATED_PROPERTIES SOURCE_PATH HEADERS SOURCES)
    STRING(REPLACE "/" "\\\\" source_group_path ${SOURCE_PATH}  )
    source_group(${source_group_path} FILES ${HEADERS} ${SOURCES})

  #-- The following is needed if we ever start to use OS X Frameworks but only
  #--  works on CMake 2.6 and greater
  #set_property(SOURCE ${HEADERS}
  #             PROPERTY MACOSX_PACKAGE_LOCATION Headers/${NAME}
  #)

ENDMACRO (IDE_GENERATED_PROPERTIES SOURCE_PATH HEADERS SOURCES)


#-------------------------------------------------------------------------------

MACRO (IDE_SOURCE_PROPERTIES SOURCE_PATH HEADERS SOURCES)

    STRING(REPLACE "/" "\\\\" source_group_path ${SOURCE_PATH}  )
    source_group(${source_group_path} FILES ${HEADERS} ${SOURCES})

  #-- The following is needed if we ever start to use OS X Frameworks but only
  #--  works on CMake 2.6 and greater
  #set_property(SOURCE ${HEADERS}
  #             PROPERTY MACOSX_PACKAGE_LOCATION Headers/${NAME}
  #)

ENDMACRO (IDE_SOURCE_PROPERTIES NAME HEADERS SOURCES)

# --------------------------------------------------------------------
#///////////////////////////////////////////////////////////////////////////////
#//
#//  Copyright (c) 2008, Michael A. Jackson. BlueQuartz Software
#//  All rights reserved.
#//  BSD License: http://www.opensource.org/licenses/bsd-license.html
#//
#///////////////////////////////////////////////////////////////////////////////
# ------------------------------------------------------------------------------ 
# This CMake code installs the needed support libraries
# ------------------------------------------------------------------------------ 
macro(InstallationSupport EXE_NAME EXE_DEBUG_EXTENSION EXE_BINARY_DIR)

    SET_TARGET_PROPERTIES( ${EXE_NAME} 
        PROPERTIES
        DEBUG_OUTPUT_NAME ${EXE_NAME}${EXE_DEBUG_EXTENSION}
        RELEASE_OUTPUT_NAME ${EXE_NAME}
    )
    
    
    INSTALL(TARGETS ${EXE_NAME} 
        BUNDLE DESTINATION .
        COMPONENT Applications
        RUNTIME DESTINATION ./
        COMPONENT Applications
        LIBRARY DESTINATION ./ 
        ARCHIVE DESTINATION ./        
    )   
    
    # --- If we are on OS X copy all the embedded libraries to the app bundle
    if(DEFINED GUI_TYPE)
       # message(STATUS "Creating Install CMake file for GUI application ${EXE_NAME}")
        if(${GUI_TYPE} STREQUAL "MACOSX_BUNDLE")
            include (${OSX_TOOLS_DIR}/OSX_BundleTools.cmake)
            if(CMAKE_BUILD_TYPE MATCHES "Debug")
                MakeOSXBundleApp( "${EXE_NAME}${EXE_DEBUG_EXTENSION}" 
                                    ${EXE_BINARY_DIR}
                                    ${OSX_TOOLS_DIR} )
            else (CMAKE_BUILD_TYPE MATCHES "Debug")
                MakeOSXBundleApp(${EXE_NAME} 
                                 ${EXE_BINARY_DIR}
                                 ${OSX_TOOLS_DIR} )
            endif() 
        endif()
    else(DEFINED GUI_TYPE)
      #  message(STATUS "Creating Install CMake file for tool application ${EXE_NAME}")
        if (APPLE)
            include (${OSX_TOOLS_DIR}/OSX_BundleTools.cmake)
            if(CMAKE_BUILD_TYPE MATCHES "Debug")
                MakeOSXTool( "${EXE_NAME}${EXE_DEBUG_EXTENSION}" 
                                    ${EXE_BINARY_DIR}
                                    ${OSX_TOOLS_DIR} )
            else (CMAKE_BUILD_TYPE MATCHES "Debug")
                MakeOSXTool(${EXE_NAME} 
                                 ${EXE_BINARY_DIR}
                                 ${OSX_TOOLS_DIR} )
            endif()
        endif(APPLE)
    endif(DEFINED GUI_TYPE)
endmacro(InstallationSupport EXE_NAME EXE_DEBUG_EXTENSION EXE_BINARY_DIR)

# --------------------------------------------------------------------
#
# --------------------------------------------------------------------
macro(ToolInstallationSupport EXE_NAME EXE_DEBUG_EXTENSION EXE_BINARY_DIR)

    SET_TARGET_PROPERTIES( ${EXE_NAME} 
        PROPERTIES
        DEBUG_OUTPUT_NAME ${EXE_NAME}${EXE_DEBUG_EXTENSION}
        RELEASE_OUTPUT_NAME ${EXE_NAME}
    )
    
    if (APPLE)
        set(TOOL_INSTALL_LOCATION "./")
    else()
        set(TOOL_INSTALL_LOCATION "./")
    endif()
    
    INSTALL(TARGETS ${EXE_NAME} 
        RUNTIME
        DESTINATION ${TOOL_INSTALL_LOCATION}
        COMPONENT Tools
        LIBRARY DESTINATION ${TOOL_INSTALL_LOCATION} 
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION ${TOOL_INSTALL_LOCATION}
        BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX}/.
    )   
    
    # --- If we are on OS X copy all the embedded libraries to the app bundle
    if(DEFINED GUI_TYPE)
      #  message(STATUS "Creating Install CMake file for GUI application ${EXE_NAME}")
        if(${GUI_TYPE} STREQUAL "MACOSX_BUNDLE")
            include (${OSX_TOOLS_DIR}/OSX_BundleTools.cmake)
            if(CMAKE_BUILD_TYPE MATCHES "Debug")
                MakeOSXBundleApp( "${EXE_NAME}${EXE_DEBUG_EXTENSION}" 
                                    ${EXE_BINARY_DIR}
                                    ${OSX_TOOLS_DIR} )
            else (CMAKE_BUILD_TYPE MATCHES "Debug")
                MakeOSXBundleApp(${EXE_NAME} 
                                 ${EXE_BINARY_DIR}
                                 ${OSX_TOOLS_DIR} )
            endif()
        endif()
    else(DEFINED GUI_TYPE)
     #   message(STATUS "Creating Install CMake file for tool application ${EXE_NAME}")
        if (APPLE)
            include (${OSX_TOOLS_DIR}/OSX_BundleTools.cmake)
            if(CMAKE_BUILD_TYPE MATCHES "Debug")
                MakeOSXTool( "${EXE_NAME}${EXE_DEBUG_EXTENSION}" 
                                    ${EXE_BINARY_DIR}
                                    ${OSX_TOOLS_DIR} )
            else (CMAKE_BUILD_TYPE MATCHES "Debug")
                MakeOSXTool(${EXE_NAME} 
                                 ${EXE_BINARY_DIR}
                                 ${OSX_TOOLS_DIR} )
            endif()
        endif(APPLE)
    endif(DEFINED GUI_TYPE)
endmacro(ToolInstallationSupport EXE_NAME EXE_DEBUG_EXTENSION EXE_BINARY_DIR)


# --------------------------------------------------------------------
#
#
macro(StaticLibraryProperties targetName )
    if (WIN32 AND NOT MINGW)
        SET (DEBUG_EXTENSION "_d")
    else()
        SET (DEBUG_EXTENSION "_debug")
    endif()
    
    
    IF (WIN32 AND NOT MINGW)
        SET(LIBRARY_RELEASE_NAME "lib${targetName}" CACHE INTERNAL "" FORCE)
        SET(LIBRARY_DEBUG_NAME "lib${targetName}${DEBUG_EXTENSION}" CACHE INTERNAL "" FORCE)
    ELSE (WIN32 AND NOT MINGW)
        SET(LIBRARY_RELEASE_NAME "${targetName}" CACHE INTERNAL "" FORCE)
        SET(LIBRARY_DEBUG_NAME "${targetName}${DEBUG_EXTENSION}" CACHE INTERNAL "" FORCE)
    ENDIF(WIN32 AND NOT MINGW)
    
    
    #-- Set the Debug and Release names for the libraries
    SET_TARGET_PROPERTIES( ${targetName} 
        PROPERTIES
        DEBUG_OUTPUT_NAME ${LIBRARY_DEBUG_NAME}
        RELEASE_OUTPUT_NAME ${LIBRARY_RELEASE_NAME}
    )

endmacro(StaticLibraryProperties)


#-------------------------------------------------------------------------------
# This macro will attempt a try_run command in order to compile and then 
# generate a version string based on today's date. The output string should be
# of the form YYYY.MM.DD. 
#  Required CMake variables to be set are:
#   EmInit_CMAKE_DIR - The path to the MXA CMake directory
#  The following variables are set, all of which should have been already
#  initialized to a default value
#   ${PROJECT_NAME}_VERSION
#   ${PROJECT_NAME}_VER_MAJOR
#   ${PROJECT_NAME}_VER_MINOR
#   ${PROJECT_NAME}_VER_PATCH
#
#-------------------------------------------------------------------------------
macro(GenerateVersionString PROJECT_NAME GENERATED_FILE_PATH NAMESPACE )
    #message(STATUS "Generating Versions String for ${PROJECT_NAME}")
    #message(STATUS "VERSION_COMPILE_FLAGS: ${VERSION_COMPILE_FLAGS}")
    
    SET (VERSION_COMPILE_FLAGS "")

    if (MSVC)
     set(HAVE_TIME_GETTIMEOFDAY "TRUE")
     set(VERSION_COMPILE_FLAGS "/DHAVE_TIME_GETTIMEOFDAY")
    endif()
     
    IF (NOT MSVC)
        TRY_COMPILE(HAVE_TIME_GETTIMEOFDAY
              ${CMAKE_BINARY_DIR}
              ${PROJECT_RESOURCES_DIR}/CMake/GetTimeOfDayTest.cpp
              COMPILE_DEFINITIONS -DTRY_TIME_H
              OUTPUT_VARIABLE OUTPUT)
        IF (HAVE_TIME_GETTIMEOFDAY STREQUAL "TRUE")
            SET (HAVE_TIME_GETTIMEOFDAY "1")
            set(VERSION_COMPILE_FLAGS "-DHAVE_TIME_GETTIMEOFDAY")
        ENDIF (HAVE_TIME_GETTIMEOFDAY STREQUAL "TRUE")
    
        TRY_COMPILE(HAVE_SYS_TIME_GETTIMEOFDAY
              ${CMAKE_BINARY_DIR}
              ${PROJECT_RESOURCES_DIR}/CMake/GetTimeOfDayTest.cpp
              COMPILE_DEFINITIONS -DTRY_SYS_TIME_H
              OUTPUT_VARIABLE OUTPUT)
        IF (HAVE_SYS_TIME_GETTIMEOFDAY STREQUAL "TRUE")
            SET (HAVE_SYS_TIME_GETTIMEOFDAY "1")
            set(VERSION_COMPILE_FLAGS "-DHAVE_SYS_TIME_GETTIMEOFDAY")
        ENDIF (HAVE_SYS_TIME_GETTIMEOFDAY STREQUAL "TRUE")
    
        if (NOT HAVE_SYS_TIME_GETTIMEOFDAY AND NOT HAVE_TIME_GETTIMEOFDAY)
            message(STATUS "---------------------------------------------------------------")
            message(STATUS "Function 'gettimeofday()' was not found. MXADataModel will use its")
            message(STATUS "  own implementation.. This can happen on older versions of")
            message(STATUS "  MinGW on Windows. Consider upgrading your MinGW installation")
            message(STATUS "  to a newer version such as MinGW 3.12")
            message(STATUS "---------------------------------------------------------------")
        endif()
    ENDIF (NOT MSVC)
    
   if ( HAVE_SYS_TIME_GETTIMEOFDAY OR HAVE_TIME_GETTIMEOFDAY)
        try_run(RUN_RESULT_VAR COMPILE_RESULT_VAR 
            ${CMAKE_CURRENT_BINARY_DIR} ${PROJECT_RESOURCES_DIR}/CMake/GenerateVersionString.cpp
            COMPILE_DEFINITIONS ${VERSION_COMPILE_FLAGS}
            COMPILE_OUTPUT_VARIABLE VERSION_COMPILE_OUTPUT
            RUN_OUTPUT_VARIABLE VERSION_GEN_COMPLETE )
           
            # and now the version string given by qmake
        STRING(REGEX REPLACE "^([0-9]+)\\.[0-9]+\\.[0-9]+.*" "\\1" VERSION_GEN_VER_MAJOR "${VERSION_GEN_COMPLETE}")
        STRING(REGEX REPLACE "^[0-9]+\\.([0-9]+)\\.[0-9]+.*" "\\1" VERSION_GEN_VER_MINOR "${VERSION_GEN_COMPLETE}")
        STRING(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_GEN_VER_PATCH "${VERSION_GEN_COMPLETE}")
        
        set (VERSION_GEN_NAME "${PROJECT_NAME}")
        set (VERSION_GEN_NAMESPACE "${NAMESPACE}")
        set (${PROJECT_NAME}_VERSION   ${VERSION_GEN_COMPLETE}  CACHE STRING "Complete Version String")
        set (${PROJECT_NAME}_VER_MAJOR ${VERSION_GEN_VER_MAJOR} CACHE STRING "Major Version String")
        set (${PROJECT_NAME}_VER_MINOR ${VERSION_GEN_VER_MINOR} CACHE STRING "Minor Version String")
        set (${PROJECT_NAME}_VER_PATCH ${VERSION_GEN_VER_PATCH} CACHE STRING "Patch Version String")
        configure_file(${PROJECT_RESOURCES_DIR}/CMake/Version.h.in
                   ${GENERATED_FILE_PATH})
    endif()
    
endmacro()

#-------------------------------------------------------------------------------
# Finds plugins from the Qt installation. The pluginlist argument should be
# something like "qgif;qjpeg;qtiff"
#-------------------------------------------------------------------------------
macro (FindQtPlugins pluginlist)
  set (qt_plugin_list ${pluginlist})
  set (qt_plugin_types "Debug;Release")
  if (WIN32)
    set(qt_plugin_prefix "")
    set(qt_plugin_DEBUG_suffix "d4")
    set(qt_plugin_RELEASE_suffix "4")
  else ()
    set(qt_plugin_prefix "lib")
    set(qt_plugin_DEBUG_suffix "_debug")
    set(qt_plugin_RELEASE_suffix "")
  endif()

  #message(STATUS "qt_plugin_debug_suffix: ${qt_plugin_debug_suffix}")
  set (QTPLUGINS_RELEASE "")
  set (QTPLUGINS_DEBUG   "")
  set (QTPLUGINS "")

  # Loop through all the Build Types and all the plugins to find each one.
  foreach(build_type ${qt_plugin_types})
  	string(TOUPPER ${build_type} BTYPE)
      foreach(plugin ${qt_plugin_list})
  	  STRING(TOUPPER ${plugin} PLUGIN)
  #      message(STATUS "|-- Looking for q${plugin}${qt_plugin_${build_type}_suffix}")
         FIND_LIBRARY( QT_${PLUGIN}_PLUGIN_${BTYPE} 
                      NAMES ${plugin}${qt_plugin_${BTYPE}_suffix} 
                      PATHS ${QT_PLUGINS_DIR}/imageformats 
                      DOC "Library Path for ${plugin}"
                      NO_DEFAULT_PATH NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
         mark_as_advanced(QT_${PLUGIN}_PLUGIN_${BTYPE})
  #      message(STATUS "|--  Q${plugin}_PLUGIN_${build_type}_LIB: ${Q${plugin}_PLUGIN_${build_type}_LIB}")
        LIST(APPEND QTPLUGINS_${BTYPE} ${QT_${PLUGIN}_PLUGIN_${BTYPE}})
      endforeach()
  endforeach()

  # Assign either the debug or release plugin list to the QTPLUGINS variable on NON msvc platforms.
  if (NOT MSVC)
      if ( NOT DEFINED CMAKE_BUILD_TYPE )
        if ( ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
            set (QTPLUGINS ${QTPLUGINS_DEBUG})
        else()
            set (QTPLUGINS ${QTPLUGINS_RELEASE})
        endif()
      else()
        set (QTPLUGINS ${QTPLUGINS_RELEASE})
      endif()
  endif()

endmacro(FindQtPlugins pluginlist)

