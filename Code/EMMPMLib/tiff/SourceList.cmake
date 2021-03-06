#--////////////////////////////////////////////////////////////////////////////
#-- Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
#-- All rights reserved.
#-- BSD License: http://www.opensource.org/licenses/bsd-license.html
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--////////////////////////////////////////////////////////////////////////////
set (EMMPM_Tiff_SRCS
    ${EMMPMLib_SOURCE_DIR}/tiff/TiffUtilities.cpp
)

set (EMMPM_Tiff_HDRS
    ${EMMPMLib_SOURCE_DIR}/tiff/TiffUtilities.h
)
cmp_IDE_SOURCE_PROPERTIES( "EMMPMLib/tiff" "${EMMPM_Tiff_HDRS}" "${EMMPM_Tiff_SRCS}" "${PROJECT_INSTALL_HEADERS}")
