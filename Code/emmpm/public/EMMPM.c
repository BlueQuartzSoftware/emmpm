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

#include <stdio.h>
#include <math.h>


#include "emmpm/private/em.h"
#include "emmpm/common/utilities/entropy.h"
#include "emmpm/common/utilities/allocate.h"
#include "emmpm/common/utilities/random.h"
#include "emmpm/common/io/EMTiffIO.h"
#include "emmpm/common/utilities/ProgressFunctions.h"
#include "emmpm/common/utilities/InitializationFunctions.h"


#if 0
/**
 * @brief Declare a global variable to hold a Callback function to show progress
 * during the EM/MPM procedure. This allows GUI applications to hook into the
 * engine in order to display progress information back to its users. We default
 * set the value to NULL in case NO progress function is ever defined.
 */
void (*EMMPM_ProgressFunc)(char*, float) = NULL;
void (*EMMPM_InitializationFunc)(EMMPM_Inputs*, EMMPM_WorkingVars*) = NULL;
void (*EMMPM_ProgressStatsFunc)(EMMPM_Update*) = NULL;
#endif


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPM_Data* EMMPM_AllocateDataStructure()
{
  EMMPM_Data* data = (EMMPM_Data*)(malloc(sizeof(EMMPM_Data)));
  int c;
   data->emIterations = 0;
   data->mpmIterations = 0;
   data->in_beta = 0.0;
   data->in_gamma = 0.0;
   data->classes = 0;
   data->rows = 0;
   data->columns = 0;
   data->channels = 0;
   data->initType = 0;
   for(c = 0; c < MAX_CLASSES; c++)
   {
     data->initCoords[c][0] = 0;
     data->initCoords[c][1] = 0;
     data->initCoords[c][2] = 0;
     data->initCoords[c][3] = 0;
     data->grayTable[c] = 0;
     data->w_gamma[c] = 0;
     data->m[c] = 0.0;
     data->v[c] = 0.0;
     data->N[c] = 0.0;
     data->probs[c] = NULL;
   }
   data->verbose = 0;


  data->input_file_name = NULL;
  data->inputImage = NULL;
  data->output_file_name = NULL;
  data->outputImage = NULL;

  data->y = NULL;
  data->xt = NULL;
  data->ga = 0;
  data->x = 0;

  data->workingBeta = 0.0;

  data->currentEMLoop = 0;
  data->currentMPMLoop = 0;
  data->progress = 0.0;
  return data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPM_CallbackFunctions* EMMPM_AllocateCallbackFunctionStructure()
{
  EMMPM_CallbackFunctions* update = (EMMPM_CallbackFunctions*)(malloc(sizeof(EMMPM_CallbackFunctions)));
  return update;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_FreeDataStructure(EMMPM_Data* data)
{
  free(data->input_file_name);
  free(data->output_file_name);
  if (data->inputImage != NULL)
  {
    EMMPM_FreeTiffImageBuffer(data->inputImage);
  }
  if(data->outputImage != NULL)
  {
    EMMPM_FreeTiffImageBuffer(data->outputImage);
  }
  free(data);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_FreeCallbackFunctionStructure(EMMPM_CallbackFunctions* ptr)
{
  free(ptr);
}
#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_ShowProgress(char* message, float progress)
{
  if (NULL != &EMMPM_ProgressFunc)
  {
    EMMPM_ProgressFunc(message, progress);
  }
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_SetProgressFunction(void (*callBack)(char*, float))
{
  EMMPM_ProgressFunc = callBack;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_SetInitializationFunction(void (*callBack)(EMMPM_Inputs*, EMMPM_WorkingVars*))
{
  EMMPM_InitializationFunc = callBack;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_SetProgressStatsFunction(void (*callBack)(EMMPM_Update*))
{
  EMMPM_ProgressStatsFunc = callBack;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_ProgressStats(EMMPM_Data* update, EMMPM_CallbackFunctions* callbacks)
{
 // EMMPM_ShowProgress("UpdateStats being called", 0.0);
  if (NULL != &callbacks->EMMPM_ProgressStatsFunc)
  {
    callbacks->EMMPM_ProgressStatsFunc(update);
  }
}
#endif
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_ConvertInputImageToWorkingImage(EMMPM_Data* data, EMMPM_CallbackFunctions* callbacks)
{
  uint8_t* dst;
  unsigned int i;
  unsigned int j;

  dst = data->inputImage;
  /* Copy input image to y[][] */
  int cols = data->columns;
  int rows = data->rows;

  data->y = (unsigned char **)get_img(cols, rows, sizeof(char));
  for (i = 0; i < rows; i++)
  {
    for (j = 0; j < cols; j++)
    {
      data->y[i][j] = *dst;
      ++dst;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_ConvertXtToOutputImage(EMMPM_Data* data, EMMPM_CallbackFunctions* callbacks)
{
  int index;
  unsigned int i;
  unsigned int j;
  unsigned char* raster;
  if (data->outputImage == NULL)
  {
    data->outputImage = EMMPM_AllocateTiffImageBuffer(data->columns, data->rows, data->channels);
  }
  raster = data->outputImage;
  index = 0;
  for (i = 0; i < data->rows; i++)
  {
    for (j = 0; j < data->columns; j++)
    {
      raster[index++] = data->grayTable[data->xt[i][j]];
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_Execute(EMMPM_Data* data, EMMPM_CallbackFunctions* callbacks)
{
  unsigned int i;
  int l;

  /* Check for a progress call back function. If it is NULL then set it to the standard
  * printf progress callback */
  if (NULL == callbacks->EMMPM_ProgressFunc)
  {
    callbacks->EMMPM_ProgressFunc = ( &EMMPM_PrintfProgress );
  }

  // Copy the input image into data->y arrays
  EMMPM_ConvertInputImageToWorkingImage(data, callbacks);

  readseed();

  for(i = 0; i < MAX_CLASSES; i++) {
    data->w_gamma[i] = data->in_gamma;
  }

  /* Allocate memory for the xt arrays */
  data->xt = (unsigned char **)get_img(data->columns, data->rows, sizeof(char));

  /* Check to make sure we have at least a basic initialization function setup */
  if (NULL == callbacks->EMMPM_InitializationFunc)
  {
    callbacks->EMMPM_InitializationFunc = &EMMPM_BasicInitialization;
  }

  /* Initialization of parameter estimation */
  callbacks->EMMPM_InitializationFunc(data);


  /* Run the EM Loops */
  EMMPM_PerformEMLoops(data, callbacks);

  /* Allocate space for the output image, and copy a scaled xt
   * and then write the output image.*/
  EMMPM_ConvertXtToOutputImage(data, callbacks);


  for (l = 0; l < data->classes; l++) {
    EMMPM_free_img((void **)data->probs[l]);
  }

  // Free all the memory that was allocated on the heap.
  EMMPM_free_img((void **)data->xt);
  EMMPM_free_img((void **)data->y);

  writeseed();
}