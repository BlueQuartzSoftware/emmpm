project (EMMPMLib)

set (EMMPMLib_PROJECT_FILES "")


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
cmp_IDE_SOURCE_PROPERTIES("emmpm/common" "${EMMPMLib_CONFIGURED_HDRS}" "${EMMPMLib_CONFIGURED_SRCS}" "${CMP_INSTALL_FILES}")

set (EMMPMLib_PROJECT_FILES ${EMMPMLib_PROJECT_FILES} ${EMMPMLib_CONFIGURED_HDRS} )

add_library(EMMPMLib ${LIB_TYPE} ${EMMPMLib_PROJECT_FILES})
target_link_libraries(EMMPMLib ${TIFF_LIBRARY} ${MATH_LIBRARY})
LibraryProperties(EMMPMLib ${EXE_DEBUG_EXTENSION} )
INSTALL(TARGETS EMMPMLib 
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin 
        COMPONENT Runtime)
        
set (lib_rt_dest lib)
if (WIN32 AND BUILD_SHARED_LIBS)
  set (lib_rt_dest bin)
endif()

# Copy DLL libraries into the correct build location on MSVC platforms
CMP_COPY_DEPENDENT_LIBRARIES ("tiff")

# Generate Install Rules for the DLL Libraries
CMP_LIBRARIES_INSTALL_RULES ("tiff" "bin" )