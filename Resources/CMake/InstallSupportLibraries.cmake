#///////////////////////////////////////////////////////////////////////////////
#//
#//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
#//  All rights reserved.
#//  BSD License: http://www.opensource.org/licenses/bsd-license.html
#//
#///////////////////////////////////////////////////////////////////////////////

# --------------------------------------------------------------------
# Copy the needed Qt libraries into the Build directory. Also add installation
# and CPack code to support installer generation.
# --------------------------------------------------------------------
if ( NOT Q_WS_MAC AND MSVC AND DEFINED QT_QMAKE_EXECUTABLE)
    
    SET (QTLIBLIST QtCore QtGui)
    SET (QTPLUGINLIST qgif qjpeg qtiff)
    set (BUILD_TYPES "Debug;Release")

    foreach (btype ${BUILD_TYPES})
        string(TOUPPER ${btype} BTYPE)
        file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${btype})
        foreach(qtlib ${QTLIBLIST})
            
            string(TOUPPER ${qtlib} QTLIB)
           # message(STATUS "QT_${QTLIB}_LIBRARY_${BTYPE}: ${QT_${QTLIB}_LIBRARY_${BTYPE}}")
            GET_FILENAME_COMPONENT(DLL_NAME ${QT_${QTLIB}_LIBRARY_${BTYPE}} NAME_WE)
           # message(STATUS "DLL_NAME: ${DLL_NAME}")
            GET_FILENAME_COMPONENT(QT_BIN_PATH ${QT_QMAKE_EXECUTABLE} PATH)
           # message(STATUS "QT_BIN_PATH: ${QT_BIN_PATH}")
            
            INSTALL(FILES ${QT_BIN_PATH}/${DLL_NAME}.dll 
                DESTINATION ./
                CONFIGURATIONS ${btype}
                COMPONENT Applications)
                
            add_custom_target(Z_${qtlib}-${BTYPE}-Copy ALL
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_BIN_PATH}/${DLL_NAME}.dll 
                        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${btype}/ 
                        COMMENT "Copying ${QT_BIN_PATH}/${DLL_NAME}.dll to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${btype}/")                    
                
        endforeach()
        
    # For this project we also need to copy the imageformat Qt plugins which should have already been defined
    # in the cmake variables QTPLUGINS_DEBUG, QTPLUGINS_RELEASE and QTPLUGINS
        file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${btype}/plugins/imageformats)     
        foreach(plugin ${QTPLUGINLIST})
            string(TOUPPER ${plugin} PLUGIN)
           # message(STATUS "QT_${QTLIB}_LIBRARY_${BTYPE}: ${QT_${QTLIB}_LIBRARY_${BTYPE}}")
            GET_FILENAME_COMPONENT(DLL_NAME ${QT_${PLUGIN}_PLUGIN_${BTYPE}} NAME_WE)
           # message(STATUS "DLL_NAME: ${DLL_NAME}")
           # GET_FILENAME_COMPONENT(QT_BIN_PATH ${QT_QMAKE_EXECUTABLE} PATH)
           # message(STATUS "QT_BIN_PATH: ${QT_BIN_PATH}")
            
            INSTALL(FILES ${QT_PLUGINS_DIR}/imageformats/${DLL_NAME}.dll 
                DESTINATION ./plugins/imageformats
                CONFIGURATIONS ${btype}
                COMPONENT Applications)
                
            add_custom_target(Z_${plugin}-${BTYPE}-Copy ALL
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_PLUGINS_DIR}/imageformats/${DLL_NAME}.dll 
                        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${btype}/plugins/imageformats/
                        COMMENT "Copying ${QT_PLUGINS_DIR}/imageformats/${DLL_NAME}.dll to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${btype}/plugins/imageformats")                    
                
        endforeach(plugin ${QTPLUGINS_${BTYPE}})
		
		#write a qt.conf file into the application directory and install it
		file(WRITE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${btype}/qt.conf "")
		INSTALL(FILES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${btype}/qt.conf
				DESTINATION .
				CONFIGURATIONS ${btype}
				COMPONENT Applications )
		
    endforeach()
endif()

# Install MinGW support libraries if those are needed
IF (MINGW)
    INCLUDE ( ${PROJECT_RESOURCES_DIR}/CMake/SHPFindMinGW.cmake )
    IF (MINGW_FOUND)
        INSTALL(FILES ${MINGW_LIBRARIES}
                DESTINATION ./
                COMPONENT Applications)
        add_custom_target(MINGW-Copy  ALL
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MINGW_LIBRARY}
                        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/ 
                        COMMENT "Copying ${MINGW_LIBRARY} to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/")
    ENDIF (MINGW_FOUND)
    
   if (DEFINED QT_QMAKE_EXECUTABLE)
        SET (QTLIBLIST QtCore QtGui)
        set(TYPE "")
        IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(TYPE "d")
        endif()
        FOREACH(qtlib ${QTLIBLIST})
          IF (WIN32)
            GET_FILENAME_COMPONENT(QT_DLL_PATH_tmp ${QT_QMAKE_EXECUTABLE} PATH)
            INSTALL(FILES ${QT_DLL_PATH_tmp}/${qtlib}${type}d4.dll 
                DESTINATION ./
                CONFIGURATIONS Debug
                COMPONENT Applications)
            INSTALL(FILES ${QT_DLL_PATH_tmp}/${qtlib}4.dll 
                DESTINATION ./
                CONFIGURATIONS Release
                COMPONENT Applications)
            add_custom_target(${qtlib}-Copy ALL
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_DLL_PATH_tmp}/${qtlib}${TYPE}4.dll
                        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/ 
                        COMMENT "Copying ${qtlib}${TYPE}4.dll to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/")

          ENDIF (WIN32)
        ENDFOREACH(qtlib)
    endif(DEFINED QT_QMAKE_EXECUTABLE)
    
ENDIF (MINGW)


