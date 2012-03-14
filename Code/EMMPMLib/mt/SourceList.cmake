#--////////////////////////////////////////////////////////////////////////////
#-- Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
#-- All rights reserved.
#-- BSD License: http://www.opensource.org/licenses/bsd-license.html
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--////////////////////////////////////////////////////////////////////////////
set (EMMPM_mt_SRCS
    ${EMMPMLib_SOURCE_DIR}/mt/mt19937ar.c
)

set (EMMPM_mt_HDRS
    ${EMMPMLib_SOURCE_DIR}/mt/mt19937ar.h
)
cmp_IDE_SOURCE_PROPERTIES( "EMMPMLib/mt" "${EMMPM_mt_HDRS}" "${EMMPM_mt_SRCS}" "${PROJECT_INSTALL_HEADERS}")
