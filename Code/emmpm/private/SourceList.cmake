set (EMMPM_Private_SRCS
    ${EMMPMLib_SOURCE_DIR}/private/em.c
    ${EMMPMLib_SOURCE_DIR}/private/mpm.c
)

set (EMMPM_Private_HDRS
    ${EMMPMLib_SOURCE_DIR}/private/em.h
    ${EMMPMLib_SOURCE_DIR}/private/mpm.h
)
cmp_IDE_SOURCE_PROPERTIES( "emmpm/private" "${EMMPM_Private_HDRS}" "${EMMPM_Private_SRCS}" "${CMP_INSTALL_FILES}")
