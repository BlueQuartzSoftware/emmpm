#--////////////////////////////////////////////////////////////////////////////
#-- Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
#-- All rights reserved.
#-- BSD License: http://www.opensource.org/licenses/bsd-license.html
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--////////////////////////////////////////////////////////////////////////////
set (EMMPM_Common_SRCS
    ${EMMPMLib_SOURCE_DIR}/Common/EMTime.cpp
    ${EMMPMLib_SOURCE_DIR}/Common/Entropy.cpp
    ${EMMPMLib_SOURCE_DIR}/Common/Observer.cpp
    ${EMMPMLib_SOURCE_DIR}/Common/Observable.cpp
    ${EMMPMLib_SOURCE_DIR}/Common/EMMPM.cpp
    ${EMMPMLib_SOURCE_DIR}/Common/EMMPMInputParser.cpp
    ${EMMPMLib_SOURCE_DIR}/Common/EMMPMUtilities.cpp
    ${EMMPMLib_SOURCE_DIR}/Common/InitializationFunctions.cpp
    ${EMMPMLib_SOURCE_DIR}/Common/EMMPM_Data.cpp
    ${EMMPMLib_SOURCE_DIR}/Common/StatsDelegate.cpp
)

set (EMMPM_Common_HDRS
    ${EMMPMLib_SOURCE_DIR}/Common/MSVCDefines.h
    ${EMMPMLib_SOURCE_DIR}/Common/EMMPMLibDLLExport.h
    ${EMMPMLib_SOURCE_DIR}/Common/EMTime.h
    ${EMMPMLib_SOURCE_DIR}/Common/Entropy.h
    ${EMMPMLib_SOURCE_DIR}/Common/EMMPM_Math.h
    ${EMMPMLib_SOURCE_DIR}/Common/Observer.h
    ${EMMPMLib_SOURCE_DIR}/Common/Observable.h
    ${EMMPMLib_SOURCE_DIR}/Common/EMMPM_Constants.h
    ${EMMPMLib_SOURCE_DIR}/Common/EMMPMUtilities.h
    ${EMMPMLib_SOURCE_DIR}/Common/EMMPM_Data.h
    ${EMMPMLib_SOURCE_DIR}/Common/EMMPM.h
    ${EMMPMLib_SOURCE_DIR}/Common/EMMPMInputParser.h
    ${EMMPMLib_SOURCE_DIR}/Common/InitializationFunctions.h
    ${EMMPMLib_SOURCE_DIR}/Common/StatsDelegate.h
)


cmp_IDE_SOURCE_PROPERTIES( "EMMPMLib/Common" "${EMMPM_Common_HDRS}" "${EMMPM_Common_SRCS}" "${PROJECT_INSTALL_HEADERS}")
