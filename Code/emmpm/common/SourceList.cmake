set (EMMPM_Common_SRCS
)

set (EMMPM_Common_HDRS
    ${EMMPMLib_SOURCE_DIR}/common/MSVCDefines.h
    ${EMMPMLib_SOURCE_DIR}/common/DLLExport.h
)



cmp_IDE_SOURCE_PROPERTIES( "emmpm/common" "${EMMPM_Common_HDRS}" "${EMMPM_Common_SRCS}" "${CMP_INSTALL_FILES}")
