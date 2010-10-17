project(Init2Class)

set(Init2Class_SRCS
${Init2Class_SOURCE_DIR}/main.c
)

add_executable(Init2Class ${Init2Class_SRCS})
target_link_libraries(Init2Class emmpmCommon)
ToolInstallationSupport(Init2Class ${EXE_DEBUG_EXTENSION} ${Init2Class_BINARY_DIR})
