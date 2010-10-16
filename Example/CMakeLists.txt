project (Example)

set (Example_HDRS
  ${Example_SOURCE_DIR}/EMMPM_Public.h
  ${Example_SOURCE_DIR}/EMMPM_Progress.h
  )

set (Example_SRCS
  ${Example_SOURCE_DIR}/example.c
  ${Example_SOURCE_DIR}/EMMPM_Public.c
  ${Example_SOURCE_DIR}/EMMPM_Progress.c
)


add_executable(example ${Example_SRCS} ${Example_HDRS})

