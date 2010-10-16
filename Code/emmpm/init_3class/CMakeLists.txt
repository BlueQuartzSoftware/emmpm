project(Init3Class)

set(Init3Class_SRCS
${Init3Class_SOURCE_DIR}/em.c
)

add_executable(Init3Class ${Init3Class_SRCS})
target_link_libraries(Init3Class emmpmCommon)
ToolInstallationSupport(Init3Class ${EXE_DEBUG_EXTENSION} ${Init3Class_BINARY_DIR})
