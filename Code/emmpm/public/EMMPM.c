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


/**
 * @brief Declare a global variable to hold a Callback function to show progress
 * during the EM/MPM procedure. This allows GUI applications to hook into the
 * engine in order to display progress information back to its users. We default
 * set the value to NULL in case NO progress function is ever defined.
 */
void (*EMMPM_ProgressFunc)(char*, float) = NULL;
void (*EMMPM_InitializationFunc)(EMMPM_Inputs*, EMMPM_WorkingVars*) = NULL;
void (*EMMPM_ProgressStatsFunc)(EMMPM_Update*) = NULL;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPM_Files* EMMPM_AllocateFilesStructure()
{
  EMMPM_Files* files = (EMMPM_Files*)(malloc(sizeof(EMMPM_Files)));
  files->input_file_name = NULL;
  files->inputImage = NULL;
  files->width = 0;
  files->height = 0;
  files->channels = 0;
  files->output_file_name = NULL;
  files->outputImage = NULL;
  return files;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPM_Inputs* EMMPM_AllocateInputsStructure()
{
  EMMPM_Inputs* inputs = (EMMPM_Inputs*)(malloc(sizeof(EMMPM_Inputs)));
  int c;
  inputs->emIterations = 0;
  inputs->mpmIterations = 0;
  inputs->beta = 0.0;
  inputs->gamma = 0.0;
  inputs->classes = 2;
  inputs->rows = 0;
  inputs->columns = 0;
  inputs->initType = 0;
  for(c = 0; c < MAX_CLASSES; c++)
  {
    inputs->initCoords[c][0] = 0;
    inputs->initCoords[c][1] = 0;
    inputs->initCoords[c][2] = 0;
    inputs->initCoords[c][3] = 0;
    inputs->grayTable[c] = 0;
  }
  inputs->verbose = 0;
  return inputs;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPM_WorkingVars* EMMPM_AllocatedWorkingVarsStructure()
{
  EMMPM_WorkingVars* vars = (EMMPM_WorkingVars*)(malloc(sizeof(EMMPM_WorkingVars)));
  return vars;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPM_Update* EMMPM_AllocateUpdateStructure()
{
  EMMPM_Update* update = (EMMPM_Update*)(malloc(sizeof(EMMPM_Update)));
  update->width = 0;
  update->height = 0;
  update->channels = 0;
  update->outputImage = NULL;
  update->classes = 0;
  update->currentEMLoop = 0;
  update->currentMPMLoop = 0;
  return update;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_FreeFilesStructure(EMMPM_Files* ptr)
{
  free(ptr->input_file_name);
  free(ptr->output_file_name);
  free(ptr);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
 void EMMPM_FreeInputsStructure(EMMPM_Inputs* ptr)
{
  free(ptr);
}
 // -----------------------------------------------------------------------------
 //
 // -----------------------------------------------------------------------------
void EMMPM_FreedWorkingVarsStructure(EMMPM_WorkingVars* ptr)
{
  free(ptr);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_FreeUpdateStructure(EMMPM_Update* ptr)
{
  free(ptr);
}

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
void EMMPM_ProgressStats(EMMPM_Update* update)
{
 // EMMPM_ShowProgress("UpdateStats being called", 0.0);
  if (NULL != &EMMPM_ProgressStatsFunc)
  {
    EMMPM_ProgressStatsFunc(update);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_ConvertInputImageToWorkingImage(EMMPM_Files* files,
                                           EMMPM_Inputs* inputs,
                                           EMMPM_WorkingVars* vars)
{
  uint8_t* dst;
  unsigned int i;
  unsigned int j;

  dst = files->inputImage;
  /* Copy input image to y[][] */
  int cols = inputs->columns;
  int rows = inputs->rows;

  vars->y = (unsigned char **)get_img(cols, rows, sizeof(char));
  for (i = 0; i < rows; i++)
  {
    for (j = 0; j < cols; j++)
    {
      vars->y[i][j] = *dst;
      ++dst;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_ConvertXtToOutputImage(EMMPM_Files* files,
                                           EMMPM_Inputs* inputs,
                                           EMMPM_WorkingVars* vars)
{

  int index;
  unsigned int i;
  unsigned int j;
  unsigned char* raster;
  if (files->outputImage == NULL)
  {
    files->outputImage = EMMPM_AllocateTiffImageBuffer(files->width, files->height, files->channels);
  }
  raster = files->outputImage;
  index = 0;
//  int value = 0;
//  int n = inputs->classes - 1;
//  unsigned char gray = 0;
  for (i = 0; i < inputs->rows; i++)
  {
    for (j = 0; j < inputs->columns; j++)
    {
//      value = vars->xt[i][j];
//      gray = value * 255 / n;
      raster[index++] = inputs->grayTable[vars->xt[i][j]];
    }
  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_Execute(EMMPM_Files* files, EMMPM_Inputs* inputs)
{
  unsigned int i;
  int l;
  EMMPM_WorkingVars* vars;

  vars = NULL;
  /* Check for a progress call back function. If it is NULL then set it to the standard
  * printf progress callback */
  if (NULL == EMMPM_ProgressFunc)
  {
    EMMPM_SetProgressFunction( &EMMPM_PrintfProgress );
  }
  vars = EMMPM_AllocatedWorkingVarsStructure();


  // Copy the input image into vars->y arrays
  EMMPM_ConvertInputImageToWorkingImage(files, inputs, vars);

  readseed();

  for(i = 0; i < MAX_CLASSES; i++) {
    vars->gamma[i] = inputs->gamma;
  }
//  vars->gamma[0] = inputs->gamma;

  /* Allocate memory for the xt arrays */
  vars->xt = (unsigned char **)get_img(inputs->columns, inputs->rows, sizeof(char));

  /* Check to make sure we have at least a basic initialization function setup */
  if (NULL == EMMPM_InitializationFunc)
  {
    EMMPM_SetInitializationFunction( &EMMPM_BasicInitialization );
  }

  /* Initialization of parameter estimation */
  EMMPM_InitializationFunc(inputs, vars);


  /* Run the EM Loops */
  EMMPM_PerformEMLoops(files, inputs, vars);

  /* Allocate space for the output image, and copy a scaled xt
   * and then write the output image.*/
  EMMPM_ConvertXtToOutputImage(files, inputs, vars);


  for (l = 0; l < inputs->classes; l++)
    EMMPM_free_img((void **)vars->probs[l]);

  // Frea all the memory that was allocated on the heap.
  EMMPM_free_img((void **)vars->xt);
  EMMPM_free_img((void **)vars->y);

  EMMPM_FreedWorkingVarsStructure(vars);

  writeseed();
}
