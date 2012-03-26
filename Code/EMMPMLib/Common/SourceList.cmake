#--////////////////////////////////////////////////////////////////////////////
#-- Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
#-- All rights reserved.
#-- BSD License: http://www.opensource.org/licenses/bsd-license.html
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--////////////////////////////////////////////////////////////////////////////
set (EMMPM_Common_SRCS
    ${EMMPMLib_SOURCE_DIR}/common/EMTime.cpp
    ${EMMPMLib_SOURCE_DIR}/common/Entropy.cpp
    ${EMMPMLib_SOURCE_DIR}/common/Observer.cpp
    ${EMMPMLib_SOURCE_DIR}/common/Observable.cpp
    ${EMMPMLib_SOURCE_DIR}/common/EMMPM.cpp
    ${EMMPMLib_SOURCE_DIR}/common/EMMPMInputParser.cpp
    ${EMMPMLib_SOURCE_DIR}/common/EMMPMUtilities.cpp
    ${EMMPMLib_SOURCE_DIR}/common/InitializationFunctions.cpp
    ${EMMPMLib_SOURCE_DIR}/common/EMMPM_Data.cpp
    ${EMMPMLib_SOURCE_DIR}/common/StatsDelegate.cpp
)

set (EMMPM_Common_HDRS
    ${EMMPMLib_SOURCE_DIR}/common/MSVCDefines.h
    ${EMMPMLib_SOURCE_DIR}/common/EMMPMLibDLLExport.h
    ${EMMPMLib_SOURCE_DIR}/common/EMTime.h
    ${EMMPMLib_SOURCE_DIR}/common/Entropy.h
    ${EMMPMLib_SOURCE_DIR}/common/EMMPM_Math.h
    ${EMMPMLib_SOURCE_DIR}/common/Observer.h
    ${EMMPMLib_SOURCE_DIR}/common/Observable.h
    ${EMMPMLib_SOURCE_DIR}/common/EMMPM_Constants.h
    ${EMMPMLib_SOURCE_DIR}/common/EMMPMUtilities.h
    ${EMMPMLib_SOURCE_DIR}/common/EMMPM_Data.h
    ${EMMPMLib_SOURCE_DIR}/common/EMMPM.h
    ${EMMPMLib_SOURCE_DIR}/common/EMMPMInputParser.h
    ${EMMPMLib_SOURCE_DIR}/common/InitializationFunctions.h
    ${EMMPMLib_SOURCE_DIR}/common/StatsDelegate.h
)

if (NOT EMMPMLib_USE_MT_RNG)
   set (EMMPM_Common_SRCS ${EMMPM_Common_SRCS}     ${EMMPMLib_SOURCE_DIR}/common/random.c )
endif()

cmp_IDE_SOURCE_PROPERTIES( "EMMPMLib/common" "${EMMPM_Common_HDRS}" "${EMMPM_Common_SRCS}" "${PROJECT_INSTALL_HEADERS}")
