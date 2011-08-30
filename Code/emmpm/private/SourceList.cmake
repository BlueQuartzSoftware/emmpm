#--////////////////////////////////////////////////////////////////////////////
#-- Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
#-- All rights reserved.
#-- BSD License: http://www.opensource.org/licenses/bsd-license.html
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--////////////////////////////////////////////////////////////////////////////
set (EMMPM_Private_SRCS
    ${EMMPMLib_SOURCE_DIR}/private/em_util.c
#    ${EMMPMLib_SOURCE_DIR}/private/mpm.c
    ${EMMPMLib_SOURCE_DIR}/private/curvature_em.c
    ${EMMPMLib_SOURCE_DIR}/private/curvature_mpm.c
    ${EMMPMLib_SOURCE_DIR}/private/morphFilt.c
)

set (EMMPM_Private_HDRS
    ${EMMPMLib_SOURCE_DIR}/private/em_util.h
#    ${EMMPMLib_SOURCE_DIR}/private/mpm.h
    ${EMMPMLib_SOURCE_DIR}/private/curvature_em.h
    ${EMMPMLib_SOURCE_DIR}/private/curvature_mpm.h
    ${EMMPMLib_SOURCE_DIR}/private/morphFilt.h
)
cmp_IDE_SOURCE_PROPERTIES( "emmpm/private" "${EMMPM_Private_HDRS}" "${EMMPM_Private_SRCS}" "${CMP_INSTALL_FILES}")
