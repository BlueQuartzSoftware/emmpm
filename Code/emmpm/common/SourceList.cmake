set (EMMPM_Common_SRCS
    ${EMMPMLib_SOURCE_DIR}/common/allocate.c
    ${EMMPMLib_SOURCE_DIR}/common/EMTime.c
    ${EMMPMLib_SOURCE_DIR}/common/entropy.c
)

set (EMMPM_Common_HDRS
    ${EMMPMLib_SOURCE_DIR}/common/MSVCDefines.h
    ${EMMPMLib_SOURCE_DIR}/common/DLLExport.h
    ${EMMPMLib_SOURCE_DIR}/common/allocate.h
    ${EMMPMLib_SOURCE_DIR}/common/EMTime.h
    ${EMMPMLib_SOURCE_DIR}/common/entropy.h
)

if (NOT EMMPM_USE_MT_RNG)
    set (EMMPM_Common_SRCS ${EMMPM_Common_SRCS} ${EMMPMLib_SOURCE_DIR}/common/random.c )
    set (EMMPM_Common_HDRS ${EMMPM_Common_HDRS} ${EMMPMLib_SOURCE_DIR}/common/random.h )
endif()

cmp_IDE_SOURCE_PROPERTIES( "emmpm/common" "${EMMPM_Common_HDRS}" "${EMMPM_Common_SRCS}" "${CMP_INSTALL_FILES}")
