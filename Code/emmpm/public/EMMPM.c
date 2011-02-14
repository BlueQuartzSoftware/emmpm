/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <stdlib.h>
#include <stdio.h>


#include "emmpm/common/entropy.h"
#include "emmpm/common/random.h"
#include "emmpm/public/ProgressFunctions.h"
#include "emmpm/public/InitializationFunctions.h"
#include "emmpm/private/em.h"
#include "emmpm/private/curvature_em.h"
#include "emmpm/tiff/EMTiffIO.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPM_Data* EMMPM_CreateDataStructure()
{
  EMMPM_Data* data = (EMMPM_Data*)(malloc(sizeof(EMMPM_Data)));
  int c;

  data->emIterations = 0;
  data->mpmIterations = 0;
  data->in_beta = 0.0;
  //  data->in_gamma = 0.0;
  data->classes = 0;
  data->rows = 0;
  data->columns = 0;
  data->dims = 1;
  data->initType = EMMPM_Basic;
  for (c = 0; c < EMMPM_MAX_CLASSES; c++)
  {
    data->initCoords[c][0] = 0;
    data->initCoords[c][1] = 1;
    data->initCoords[c][2] = 2;
    data->initCoords[c][3] = 3;
    data->grayTable[c] = 0;
    data->N[c] = 0.0;
    data->w_gamma[c] = 1.0;
  }
  data->verbose = 0;


  data->m = NULL;
  data->v = NULL;
  data->probs = NULL;
  data->ccost = NULL;

  data->input_file_name = NULL;
  data->inputImage = NULL;
  data->inputImageChannels = 0;
  data->output_file_name = NULL;
  data->outputImage = NULL;

  data->y = NULL;
  data->xt = NULL;
  data->workingBeta = 0.0;

  data->currentEMLoop = 0;
  data->currentMPMLoop = 0;
  data->progress = 0.0;
  data->userData = NULL;
  data->inside_em_loop = 0;
  data->inside_mpm_loop = 0;

  data->useCurvaturePenalty = 0;
  data->ccostLoopDelay = 1;
  data->beta_e = 0.0;
  data->beta_c = 0.0;
  data->r_max = 0.0;
  data->ns = NULL;
  data->ew = NULL;
  data->sw = NULL;
  data->nw = NULL;

  return data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EMMPM_AllocateDataStructureMemory(EMMPM_Data* data)
{
  data->y = (unsigned char *)malloc(data->columns * data->rows * data->dims * sizeof(unsigned char));
  if (NULL == data->y) return -1;
  data->xt = (unsigned char*)malloc(data->columns * data->rows * sizeof(unsigned char));
  if (NULL == data->xt) return -1;
//  data->w_gamma = (double*)malloc(data->classes * data->dims * sizeof(double));
//  if (NULL == data->w_gamma) return -1;
  data->m = (double*)malloc(data->classes * data->dims * sizeof(double));
  if (NULL == data->m) return -1;
  data->v = (double*)malloc(data->classes * data->dims * sizeof(double));
  if (NULL == data->v) return -1;
//  data->N = (double*)malloc(data->classes * data->dims * sizeof(double));
//  if (NULL == data->N) return -1;
  data->probs = (double*)malloc(data->classes * data->columns * data->rows * sizeof(double));
  if (NULL == data->probs) return -1;

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPM_CallbackFunctions* EMMPM_AllocateCallbackFunctionStructure()
{
  EMMPM_CallbackFunctions* callbacks = (EMMPM_CallbackFunctions*)(malloc(sizeof(EMMPM_CallbackFunctions)));
  callbacks->EMMPM_InitializationFunc = NULL;
  callbacks->EMMPM_ProgressFunc = NULL;
  callbacks->EMMPM_ProgressStatsFunc = NULL;

  return callbacks;
}

#define EMMPM_FREE_POINTER(ptr)\
  if (NULL != (ptr)) { free(ptr); (ptr) = NULL;}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_FreeDataStructure(EMMPM_Data* data)
{
  EMMPM_FREE_POINTER(data->input_file_name)
  EMMPM_FREE_POINTER(data->output_file_name)

  if (data->inputImage != NULL)
  {
    EMMPM_FreeTiffImageBuffer(data->inputImage);
    data->inputImage = NULL;
  }
  if(data->outputImage != NULL)
  {
    EMMPM_FreeTiffImageBuffer(data->outputImage);
    data->outputImage = NULL;
  }

  EMMPM_FREE_POINTER(data->y)
  EMMPM_FREE_POINTER(data->xt)
  EMMPM_FREE_POINTER(data->m)
  EMMPM_FREE_POINTER(data->v)
  EMMPM_FREE_POINTER(data->probs)
  EMMPM_FREE_POINTER(data->ccost)
  EMMPM_FREE_POINTER(data->ns)
  EMMPM_FREE_POINTER(data->ew)
  EMMPM_FREE_POINTER(data->sw)
  EMMPM_FREE_POINTER(data->nw)

  free(data);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_FreeCallbackFunctionStructure(EMMPM_CallbackFunctions* ptr)
{
  free(ptr);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_ConvertInputImageToWorkingImage(EMMPM_Data* data, EMMPM_CallbackFunctions* callbacks)
{
  uint8_t* dst;
  int i;
  int j;
  int d;
  size_t index = 0;
  int width;
  int height;
  int dims;

  if (data->inputImageChannels == 0)
  {
    printf("The number of input color channels was 0\n. Exiting Program.\n");
    exit(1);
  }

  if (data->inputImageChannels != data->dims)
  {
    printf("The number of image channels does not match the number of vector dimensions\nExiting Program.\n");
    exit(1);
  }


  /* Copy input image to y[][] */
  width = data->columns;
  height = data->rows;
  dims = data->dims;
  dst = data->inputImage;


  for (j = 0; j < height; j++)
  {
    for (i = 0; i < width; i++)
    {
      for (d = 0; d < dims; d++)
      {
        index = (j*width*dims) + (i*dims) + d;
        data->y[index] = *dst;
        ++dst;
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_ConvertXtToOutputImage(EMMPM_Data* data, EMMPM_CallbackFunctions* callbacks)
{
  size_t index;
  unsigned int i;
  unsigned int j;
  unsigned char* raster;
  size_t gtindex = 0;


  if (data->outputImage == NULL)
  {
    data->outputImage = EMMPM_AllocateTiffImageBuffer(data->columns, data->rows, data->dims);
  }
  raster = data->outputImage;
  index = 0;
  for (i = 0; i < data->rows; i++)
  {
    for (j = 0; j < data->columns; j++)
    {
      gtindex = data->xt[ i*data->columns + j ];
      raster[index++] = data->grayTable[gtindex];
    }
  }
}


#define PRINT_DATA(var)\
  printf("%s: %d\n", #var, data->var);

#define PRINT_DATA_DOUBLE(var)\
  printf("%s: %f\n", #var, data->var);

#define PRINT_DATA_CHAR(var)\
  printf("%s: %s\n", #var, data->var);


#define PRINT_CHAR_ARRAY(var)\
    printf("%s[MAX_CLASSES]; ", #var);\
    for (i = 0; i < EMMPM_MAX_CLASSES; i++) {\
      printf("%d  ", data->var[i]);}\
      printf("\n");


#define PRINT_DOUBLE_ARRAY(var)\
    printf("%s[MAX_CLASSES]; ", #var);\
    for (i = 0; i < EMMPM_MAX_CLASSES; i++){ \
      printf("%f  ", data->var[i]);}\
    printf("\n");

#define PRINT_INT_ARRAY(var)\
    printf("%s[MAX_CLASSES]; ", #var);\
    for (i = 0; i < EMMPM_MAX_CLASSES; i++){ \
      printf("%d  ", data->var[i]);}\
      printf("\n");

#define PRINT_UINT_ARRAY(var)\
    {printf("%s[MAX_CLASSES]; ", #var);\
    unsigned int ui;\
    for (i = 0; i < EMMPM_MAX_CLASSES; i++){ \
      ui = data->var[i];\
      printf("%u  ", ui);}\
      printf("\n");}

#define PRINT_2D_UINT_ARRAY(var, r, c)\
    {printf("%s[%s][%s];\n  ", #var, #r, #c);\
    for (i = 0; i < r; i++){ \
      for (j = 0; j < c; j++) {\
      ui = data->var[i][j];\
      printf("%u  ", ui);}\
      printf("\n  ");} }

#define PRINT_POINTER(var)\
    printf("%p\n", data->var);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

void printData(EMMPM_Data* data)
{
#if 0
  int i = 0;
  int j = 0;
  unsigned int ui;
  PRINT_DATA(emIterations)
  PRINT_DATA( mpmIterations); /**<  */
  PRINT_DATA_DOUBLE( in_beta); /**<  */
 // PRINT_DATA_DOUBLE( in_gamma); /**<  */
  PRINT_DATA( classes); /**<  */
  PRINT_DATA( rows); /**< The height of the image.  Applicable for both input and output images */
  PRINT_DATA( columns); /**< The width of the image. Applicable for both input and output images */
  PRINT_DATA( channels); /**< The number of color channels in the images. This should always be 1 */
  PRINT_DATA( initType); /**< The type of initialization algorithm to use  */
  PRINT_2D_UINT_ARRAY( initCoords, EMMPM_MAX_CLASSES, 4); /**<  MAX_CLASSES rows x 4 Columns  */

  PRINT_DATA( simulatedAnnealing); /**<  */

  PRINT_INT_ARRAY( grayTable);
  PRINT_DATA( verbose); /**<  */

  PRINT_DATA_CHAR( input_file_name);/**< The input file name */
  PRINT_POINTER( inputImage); /**< The raw image data that is used as input to the algorithm */
  PRINT_DATA_CHAR( output_file_name); /**< The name of the output file */
  PRINT_POINTER( outputImage); /**< The raw output image data which can be allocated by the library or the calling function. */

  PRINT_DATA_DOUBLE( y); /**<  */
  PRINT_DATA_DOUBLE( xt); /**<  */
  PRINT_DOUBLE_ARRAY( w_gamma); /**<  */

//  PRINT_DATA_DOUBLE( x); /**<  */
  PRINT_DOUBLE_ARRAY( m); /**<  */
  PRINT_DOUBLE_ARRAY( v); /**<  */
  PRINT_DOUBLE_ARRAY( N); /**<  */
  PRINT_DOUBLE_ARRAY( probs); /**<  */
  PRINT_DATA_DOUBLE( workingBeta); /**<  */

  PRINT_DATA( currentEMLoop); /**< The current EM Loop  */
  PRINT_DATA( currentMPMLoop); /**< The current MPM Loop  */
  PRINT_DATA_DOUBLE( progress); /**< A Percentage to indicate how far along the algorthm is.*/

  PRINT_DATA(userData); /**< User defined Pointer that can point to anything */
#endif
}




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_Run(EMMPM_Data* data, EMMPM_CallbackFunctions* callbacks)
{
  // Copy the input image into data->y arrays
  EMMPM_ConvertInputImageToWorkingImage(data, callbacks);
  init_genrand(143542612ul);

  /* Check to make sure we have at least a basic initialization function setup */
  if (NULL == callbacks->EMMPM_InitializationFunc)
  {
    callbacks->EMMPM_InitializationFunc = &EMMPM_BasicInitialization;
  }


  /* Initialization of parameter estimation */
  callbacks->EMMPM_InitializationFunc(data);

  if (data->useCurvaturePenalty != 0)
  {
    /* Initialize the Curvature Penalty variables */
    EMMPM_InitCurvatureVariables(data);
  }

#if 0
  /* Allocate space for the output image, and copy a scaled xt
   * and then write the output image.*/
  EMMPM_ConvertXtToOutputImage(data, callbacks);
  char* oimagefname = data->output_file_name;
  data->output_file_name = "/tmp/Initial_Random_Image.tif";
  EMMPM_WriteGrayScaleTiff(data, callbacks, "Initial Random Image");
  data->output_file_name = oimagefname;
#endif

  printData(data);

  if (data->useCurvaturePenalty)
  {
    EMMPM_CurvatureEMLoops(data, callbacks);
  }
  else
  {
    EMMPM_PerformEMLoops(data, callbacks);
  }

  /* Allocate space for the output image, and copy a scaled xt
   * and then write the output image.*/
  EMMPM_ConvertXtToOutputImage(data, callbacks);


//  writeseed();
}

