#--////////////////////////////////////////////////////////////////////////////
#-- Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
#-- All rights reserved.
#-- BSD License: http://www.opensource.org/licenses/bsd-license.html
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--////////////////////////////////////////////////////////////////////////////
set (EMMPM_OCL_SRCS
    ${EMMPMLib_SOURCE_DIR}/ocl/OpenCLUtils.c
)

set (EMMPM_OCL_HDRS
    ${EMMPMLib_SOURCE_DIR}/ocl/OpenCLUtils.h
)
cmp_IDE_SOURCE_PROPERTIES( "emmpm/ocl" "${EMMPM_OCL_HDRS}" "${EMMPM_OCL_SRCS}" "${CMP_INSTALL_FILES}")


