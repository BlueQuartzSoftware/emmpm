set (EMMPM_mt_SRCS
    ${EMMPMLib_SOURCE_DIR}/mt/mt19937ar.c
)

set (EMMPM_mt_HDRS
    ${EMMPMLib_SOURCE_DIR}/mt/mt19937ar.h
)
cmp_IDE_SOURCE_PROPERTIES( "emmpm/mt" "${EMMPM_mt_HDRS}" "${EMMPM_mt_SRCS}" "${CMP_INSTALL_FILES}")
