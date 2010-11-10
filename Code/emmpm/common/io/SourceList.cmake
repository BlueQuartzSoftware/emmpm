set (EMMPM_Common_io_SRCS
    ${EMMPMLib_SOURCE_DIR}/common/io/EMTiffIO.c
)

set (EMMPM_Common_io_HDRS
    ${EMMPMLib_SOURCE_DIR}/common/io/EMTiffIO.h
)
cmp_IDE_SOURCE_PROPERTIES( "emmpm/common/io" "${EMMPM_Common_io_HDRS}" "${EMMPM_Common_io_SRCS}" "${CMP_INSTALL_FILES}")
