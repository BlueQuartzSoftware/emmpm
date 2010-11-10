project(Init3ClassWrite2)

set(Init3ClassWrite2_SRCS
${Init3ClassWrite2_SOURCE_DIR}/em.c
)

add_executable(Init3ClassWrite2 ${Init3ClassWrite2_SRCS})
target_link_libraries(Init3ClassWrite2 emmpmCommon)
ToolInstallationSupport(Init3ClassWrite2 ${EXE_DEBUG_EXTENSION} ${Init3ClassWrite2_BINARY_DIR})
