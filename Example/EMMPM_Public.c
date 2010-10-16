
#include "EMMPM_Public.h"

/**
 * @brief Declare a global variable to hold a Callback function to show progress
 * during the EM/MPM procedure. This allows GUI applications to hook into the
 * engine in order to display progress information back to its users. We default
 * set the value to NULL in case NO progress function is ever defined.
 */
void (*EMMPM_ProgressFunc)(char*, float) = NULL;

/* --------------------------------------------------------------------------- */
EMMPM_Parameters* Allocate_EMMPM_Parameters()
{
  EMMPM_Parameters* params;
  params = malloc(sizeof( EMMPM_Parameters));
  if (NULL != params)
  {
    params->inputImage = NULL;
    params->outputImage = NULL;
    params->imageWidth = 0;
    params->imageHeight = 0;
    params->beta = 1.0;
    params->gamma = 0.1;
    params->emIterations = 0;
    params->mpmIterations = 0;
    params->errorCode = 0;
  }
  return params;
}

/* --------------------------------------------------------------------------- */
void Free_EMMPM_Parameters(EMMPM_Parameters* parameters)
{
  free(parameters);
}

/* --------------------------------------------------------------------------- */
void EMMPM_setProgressFunction(void (*callBack)(char*, float))
{
  EMMPM_ProgressFunc = callBack;
}

/* --------------------------------------------------------------------------- */
int EMMPM_execute(EMMPM_Parameters* parameters)
{
  if (NULL != EMMPM_ProgressFunc) {
    EMMPM_ProgressFunc("Testing from Progress Callback", 0.9f);
  }
  return 0;
}
