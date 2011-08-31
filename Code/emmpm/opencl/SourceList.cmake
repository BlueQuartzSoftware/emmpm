#--////////////////////////////////////////////////////////////////////////////
#-- Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
#-- All rights reserved.
#-- BSD License: http://www.opensource.org/licenses/bsd-license.html
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--////////////////////////////////////////////////////////////////////////////
set (EMMPM_OCL_SRCS
    ${EMMPMLib_SOURCE_DIR}/opencl/OpenCLUtils.c
    ${EMMPMLib_SOURCE_DIR}/opencl/opencl_mpm.c
)

set (EMMPM_OCL_HDRS
    ${EMMPMLib_SOURCE_DIR}/opencl/OpenCLUtils.h
    ${EMMPMLib_SOURCE_DIR}/opencl/opencl_mpm.c
    ${EMMPMLib_SOURCE_DIR}/opencl/ocl_mpm_cpu.cl
)
cmp_IDE_SOURCE_PROPERTIES( "emmpm/opencl" "${EMMPM_OCL_HDRS}" "${EMMPM_OCL_SRCS}" "${CMP_INSTALL_FILES}")


