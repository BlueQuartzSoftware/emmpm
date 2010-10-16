project(Init4Class)

set(Init4Class_SRCS
${Init4Class_SOURCE_DIR}/em.c
)

add_executable(Init4Class ${Init4Class_SRCS})
target_link_libraries(Init4Class emmpmCommon)
ToolInstallationSupport(Init4Class ${EXE_DEBUG_EXTENSION} ${Init4Class_BINARY_DIR})
