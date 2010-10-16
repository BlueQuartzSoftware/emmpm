project (EMMPMCommon)


set (EMMPMCommon_SRCS
  ${EMMPMCommon_SOURCE_DIR}/allocate.c
  ${EMMPMCommon_SOURCE_DIR}/EMTiffIO.c
  ${EMMPMCommon_SOURCE_DIR}/EMTime.c
  ${EMMPMCommon_SOURCE_DIR}/entropy.c
  ${EMMPMCommon_SOURCE_DIR}/mpm.c
  ${EMMPMCommon_SOURCE_DIR}/random.c
)

set (EMMPMCommon_HDRS
  ${EMMPMCommon_SOURCE_DIR}/allocate.h
  ${EMMPMCommon_SOURCE_DIR}/EMTiffIO.h
  ${EMMPMCommon_SOURCE_DIR}/EMTime.h
#  ${EMMPMCommon_SOURCE_DIR}/entropy.h
#  ${EMMPMCommon_SOURCE_DIR}/mpm.h
  ${EMMPMCommon_SOURCE_DIR}/random.h
  ${EMMPMCommon_SOURCE_DIR}/MSVCDefines.h
)

IDE_SOURCE_PROPERTIES( Code "emmpm/Common/${EMMPMCommon_HDRS}" "emmpm/Common/${EMMPMCommon_SRCS}")

add_library(emmpmCommon STATIC ${EMMPMCommon_SRCS} ${EMMPMCommon_HDRS})
target_link_libraries(emmpmCommon ${TIFF_LIBRARY} ${MATH_LIBRARY})
StaticLibraryProperties(emmpmCommon)
