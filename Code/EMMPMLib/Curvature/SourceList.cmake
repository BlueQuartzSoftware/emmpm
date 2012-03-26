#--////////////////////////////////////////////////////////////////////////////
#-- Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
#-- All rights reserved.
#-- BSD License: http://www.opensource.org/licenses/bsd-license.html
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--////////////////////////////////////////////////////////////////////////////
set (EMMPM_Curvature_SRCS
    
    ${EMMPMLib_SOURCE_DIR}/Curvature/CurvatureEM.cpp
    ${EMMPMLib_SOURCE_DIR}/Curvature/CurvatureMPM.cpp
    ${EMMPMLib_SOURCE_DIR}/Curvature/MorphFilt.cpp
)

set (EMMPM_Curvature_HDRS
    
    ${EMMPMLib_SOURCE_DIR}/Curvature/CurvatureEM.h
    ${EMMPMLib_SOURCE_DIR}/Curvature/CurvatureMPM.h
    ${EMMPMLib_SOURCE_DIR}/Curvature/MorphFilt.h
)
cmp_IDE_SOURCE_PROPERTIES( "EMMPMLib/Curvature" "${EMMPM_Curvature_HDRS}" "${EMMPM_Curvature_SRCS}" "${PROJECT_INSTALL_HEADERS}")
