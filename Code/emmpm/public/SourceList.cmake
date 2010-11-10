set (EMMPM_Public_SRCS
   ${EMMPMLib_SOURCE_DIR}/public/EMMPM.c
   ${EMMPMLib_SOURCE_DIR}/public/EMMPMInputParser.cpp
)

set (EMMPM_Public_HDRS
   ${EMMPMLib_SOURCE_DIR}/public/EMMPM_Constants.h
   ${EMMPMLib_SOURCE_DIR}/public/EMMPM_Structures.h
   ${EMMPMLib_SOURCE_DIR}/public/EMMPM.h
   ${EMMPMLib_SOURCE_DIR}/public/EMMPMInputParser.h
)
cmp_IDE_SOURCE_PROPERTIES( "emmpm/public" "${EMMPM_Public_HDRS}" "${EMMPM_Public_SRCS}" "${CMP_INSTALL_FILES}")
