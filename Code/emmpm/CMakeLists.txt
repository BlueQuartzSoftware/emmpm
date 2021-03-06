#--////////////////////////////////////////////////////////////////////////////
#-- Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
#-- All rights reserved.
#-- BSD License: http://www.opensource.org/licenses/bsd-license.html
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--////////////////////////////////////////////////////////////////////////////
project (EMMPMLib)

set (EMMPMLib_PROJECT_FILES "")

#-- Add an executable which will convert the .cl files into C Header files
add_executable(OpenCLCodeToHeader ${EMMPMLib_SOURCE_DIR}/opencl/OpenCLCodeToHeader.c)


#-- Make sure the output directory is created/exists
file(MAKE_DIRECTORY ${emmpm_BINARY_DIR}/emmpm/opencl/)
#-- Add a custom command to convert the .cl file into a header file
#-- If we have multiple .cl files then we need to do this for each .cl file
add_custom_command(OUTPUT ${emmpm_BINARY_DIR}/emmpm/opencl/ocl_mpm_cpu.h 
                    DEPENDS ${EMMPMLib_SOURCE_DIR}/opencl/ocl_mpm_cpu.cl 
                    COMMAND OpenCLCodeToHeader ${EMMPMLib_SOURCE_DIR}/opencl/ocl_mpm_cpu.cl 
                                        ${emmpm_BINARY_DIR}/emmpm/opencl/ocl_mpm_cpu.h 
                    COMMENT "Creating Header File from OpenCL ${EMMPMLib_SOURCE_DIR}/opencl/ocl_mpm_cpu.cl")

# --------------------------------------------------------------------
Macro(Add_EMMPMLib_Dir VarName Path)
    include(${EMMPMLib_SOURCE_DIR}/${Path}/SourceList.cmake)
    set (EMMPMLib_PROJECT_FILES ${EMMPMLib_PROJECT_FILES}
                                        ${EMMPM_${VarName}_SRCS}
                                        ${EMMPM_${VarName}_HDRS} )
endMacro()


Add_EMMPMLib_Dir(Public public)
Add_EMMPMLib_Dir(Common common)
Add_EMMPMLib_Dir(OCL opencl)
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

set (EMMPMLib_PROJECT_FILES 
     ${EMMPMLib_PROJECT_FILES} 
     ${EMMPMLib_CONFIGURED_HDRS}
     ${emmpm_BINARY_DIR}/emmpm/opencl/ocl_mpm_cpu.h
      )

INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_DIRS} )
add_library(EMMPMLib ${LIB_TYPE} ${EMMPMLib_PROJECT_FILES})

if ( ${OPENCL_FOUND} )
    set (MATH_LIBRARY ${MATH_LIBRARY} ${OPENCL_LIBRARIES})
endif()

message(STATUS "OPENCL_INCLUDE_DIRS: ${OPENCL_INCLUDE_DIRS}")


target_link_libraries(EMMPMLib ${TIFF_LIBRARY} ${MATH_LIBRARY})
LibraryProperties(EMMPMLib ${EXE_DEBUG_EXTENSION} )
INSTALL(TARGETS EMMPMLib 
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin 
        COMPONENT Applications)
        
set (lib_rt_dest lib)
if (WIN32 AND BUILD_SHARED_LIBS)
  set (lib_rt_dest bin)
endif()

# Copy DLL libraries into the correct build location on MSVC platforms
CMP_COPY_DEPENDENT_LIBRARIES ("tiff")

# Generate Install Rules for the DLL Libraries
CMP_LIBRARIES_INSTALL_RULES ("tiff" "bin" )

