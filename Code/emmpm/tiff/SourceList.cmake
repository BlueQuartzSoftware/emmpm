set (EMMPM_Tiff_SRCS
    ${EMMPMLib_SOURCE_DIR}/tiff/EMTiffIO.c
)

set (EMMPM_Tiff_HDRS
    ${EMMPMLib_SOURCE_DIR}/tiff/EMTiffIO.h
)
cmp_IDE_SOURCE_PROPERTIES( "emmpm/tiff" "${EMMPM_Tiff_HDRS}" "${EMMPM_Tiff_SRCS}" "${CMP_INSTALL_FILES}")
