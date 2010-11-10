project(Init4ClassWrite2)

set(Init4ClassWrite2_SRCS
${Init4ClassWrite2_SOURCE_DIR}/em.c
)

add_executable(Init4ClassWrite2 ${Init4ClassWrite2_SRCS})
target_link_libraries(Init4ClassWrite2 emmpmCommon)
ToolInstallationSupport(Init4ClassWrite2 ${EXE_DEBUG_EXTENSION} ${Init4ClassWrite2_BINARY_DIR})
