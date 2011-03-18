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
