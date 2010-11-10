set (EMMPM_Common_utilities_SRCS
    ${EMMPMLib_SOURCE_DIR}/common/utilities/allocate.c
${EMMPMLib_SOURCE_DIR}/common/utilities/EMTime.c
${EMMPMLib_SOURCE_DIR}/common/utilities/entropy.c
${EMMPMLib_SOURCE_DIR}/common/utilities/InitializationFunctions.c
${EMMPMLib_SOURCE_DIR}/common/utilities/ProgressFunctions.c
${EMMPMLib_SOURCE_DIR}/common/utilities/random.c
)

set (EMMPM_Common_utilities_HDRS
${EMMPMLib_SOURCE_DIR}/common/utilities/allocate.h
${EMMPMLib_SOURCE_DIR}/common/utilities/EMTime.h
${EMMPMLib_SOURCE_DIR}/common/utilities/entropy.h
${EMMPMLib_SOURCE_DIR}/common/utilities/InitializationFunctions.h
${EMMPMLib_SOURCE_DIR}/common/utilities/ProgressFunctions.h
${EMMPMLib_SOURCE_DIR}/common/utilities/random.h
)
cmp_IDE_SOURCE_PROPERTIES( "emmpm/common/utilities" "${EMMPM_Common_utilities_HDRS}" 
"${EMMPM_Common_utilities_SRCS}" "${CMP_INSTALL_FILES}")
