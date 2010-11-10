#ifndef EMMPM_PROGRESS_H
#define EMMPM_PROGRESS_H

/* Use these definitions to allow the code to be included in C++ code */
#ifdef __cplusplus
  extern "C" {
#endif

/**
* @brief
* @param message A message to pass back to the calling program
* @param progress A value between 0 and 1 that represents the percentage completed
*/
void ProgressCallBack(char* message, float progress);

#ifdef __cplusplus
  }
#endif


#endif /* end of include guard: EMMPM_PROGRESS_H */
