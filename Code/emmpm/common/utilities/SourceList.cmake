set (EMMPM_Common_utilities_SRCS
    ${EMMPMLib_SOURCE_DIR}/common/utilities/allocate.c
    ${EMMPMLib_SOURCE_DIR}/common/utilities/EMTime.c
    ${EMMPMLib_SOURCE_DIR}/common/utilities/entropy.c
)

set (EMMPM_Common_utilities_HDRS
    ${EMMPMLib_SOURCE_DIR}/common/utilities/allocate.h
    ${EMMPMLib_SOURCE_DIR}/common/utilities/EMTime.h
    ${EMMPMLib_SOURCE_DIR}/common/utilities/entropy.h
)

if (NOT EMMPM_USE_MT_RNG)
    set (EMMPM_Common_utilities_SRCS ${EMMPM_Common_utilities_SRCS} ${EMMPMLib_SOURCE_DIR}/common/utilities/random.c )
    set (EMMPM_Common_utilities_HDRS ${EMMPM_Common_utilities_HDRS} ${EMMPMLib_SOURCE_DIR}/common/utilities/random.h )
endif()

cmp_IDE_SOURCE_PROPERTIES( "emmpm/common/utilities" "${EMMPM_Common_utilities_HDRS}" 
"${EMMPM_Common_utilities_SRCS}" "${CMP_INSTALL_FILES}")
