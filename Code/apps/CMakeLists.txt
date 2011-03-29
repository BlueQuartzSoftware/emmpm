project(emmpm)

set(emmpm_SRCS
    ${emmpm_SOURCE_DIR}/main.cpp
)

set(emmpm_HDRS
)

cmp_IDE_SOURCE_PROPERTIES("emmpm/apps" "${emmpm_HDRS}" "${emmpm_SRCS}"  "${CMP_INSTALL_FILES}")

add_executable(emmpm ${emmpm_SRCS})
target_link_libraries(emmpm EMMPMLib)
cmp_ToolInstallationSupport(emmpm ${EXE_DEBUG_EXTENSION} ${emmpm_BINARY_DIR})
