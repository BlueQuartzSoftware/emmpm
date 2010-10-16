/*
Additional items that might be considered would be the following functions:

ProgressImage() - which would be a function that would send an "image" that represents
the current step in the segmentation. Nice to be able to track the evolution of
the segmentation in a gui or through writing the image to disk

InitializationFunction() - Allow the programmer to set a specific initialization function
that they have written instead of the standard algorithm

*/

#ifndef EMMPM_PUBLIC_H
#define EMMPM_PUBLIC_H

#include <stdlib.h>

/* Use these definitions to allow the code to be included in C++ code */
#ifdef __cplusplus
  extern "C" {
#endif

/**
* @enum EMMPM_ErrorCodes List of the types of error codes that could be encountered
* by the em/mpm algorithm
*/
enum EMMPM_ErrorCodes {
  NoError = 0,
  Error = 1,
};

/**
* @struct EMMPM_Parameters 
*/
typedef struct
{
  unsigned char* inputImage;
  unsigned char* outputImage;
  size_t        imageWidth;
  size_t        imageHeight;
  float         beta;
  float         gamma;
  int           emIterations;
  int           mpmIterations;
  int           errorCode;
} EMMPM_Parameters;

/**
* @brief
* @return
*/
EMMPM_Parameters* Allocate_EMMPM_Parameters();

/**
* @brief Frees the structure but NOT the memory associated with the input
* and output images.
* @param parameters
*/
void Free_EMMPM_Parameters(EMMPM_Parameters* parameters);

/**
* @brief Sets a function to be used to provide progress information in the form
* of a text variable and a percent variable which should range between zero and
* one
* @param *pt2func The function to be called for progress feedback
*/
void EMMPM_setProgressFunction(void (*pt2Func)(char*, float));

/**
* @brief
* @param
* @return
*/
void EMMPM_setInitialzationFunction(void (*pt2Func)(void*));

/**
* @brief Executes the EM/MPM algorithm using the given parameters.
* @param parameters The EM/MPM Input/Output Parameters
* @return Error code 
*/
int EMMPM_execute(EMMPM_Parameters* parameters);

#ifdef __cplusplus
  }
#endif



#endif /* end of include guard: EMMPM_PUBLIC_H */
