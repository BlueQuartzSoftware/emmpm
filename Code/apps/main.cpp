/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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

#include <string.h>
#include <stdlib.h>


#include <iostream>
#include <ostream>
#include <fstream>
#include <string>

#include "emmpm/common/EMMPM_Math.h"
#include "emmpm/common/EMMPMTypes.h"
#include "emmpm/public/EMMPM_Structures.h"
#include "emmpm/public/EMMPM.h"
#include "emmpm/public/EMMPMInputParser.h"
#include "emmpm/public/ProgressFunctions.h"
#include "emmpm/public/InitializationFunctions.h"
#include "emmpm/tiff/EMTiffIO.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateStats(EMMPM_Data* data)
{
  // Check to make sure we are at the end of an em loop
  if ( data->inside_mpm_loop == 0 && NULL != data->outputImage)
  {
    char buff[256];
    memset(buff, 0, 256);
#if (_WIN32)
    snprintf(buff, 256, "C:\\Data\\emmpm_out_%d.tif", data->currentEMLoop);
#else
    snprintf(buff, 256, "/tmp/emmpm_out_%d.tif", data->currentEMLoop);
#endif
    std::cout << "Writing Image: " << buff << std::endl;
    int err = EMMPM_WriteGrayScaleImage(buff, data->rows, data->columns, "Intermediate Image", data->outputImage);
    if (err < 0)
    {
      std::cout << "Error writing intermediate tiff image." << std::endl;
    }

    std::cout << "Class\tMu\tSigma" << std::endl;
    for (int l = 0; l < data->classes; l++)
    {
      //    snprintf(msgbuff, 256, "%d\t%.3f\t%.3f", l, data->m[l], data->v[l]);
      //    EMMPM_ShowProgress(msgbuff, data->progress);
      std::cout << l << "\t" << data->m[l] << "\t" << data->v[l] << "\t" << std::endl;
    }
#if 0
    double hist[EMMPM_MAX_CLASSES][256];
    // Generate a gaussian curve for each class based off the mu and sigma for that class
    for (int c = 0; c < data->classes; ++c)
    {
      double mu = data->m[c];
      double sig = data->v[c];
      double twoSigSqrd = sig * sig * 2.0f;
      double constant = 1.0f / (sig * sqrtf(2.0f * M_PI));
      for (size_t x = 0; x < 256; ++x)
      {
        hist[c][x] = constant * exp(-1.0f * ((x - mu) * (x - mu)) / (twoSigSqrd));
      }
    }


    memset(buff, 0, 256);
    snprintf(buff, 256, "/tmp/emmpm_hist_%d.csv", data->currentEMLoop);
    std::ofstream file(buff, std::ios::out | std::ios::binary);
    if (file.is_open())
    {
      for (size_t x = 0; x < 256; ++x)
      {
        file << x;
        for (int c = 0; c < data->classes; ++c)
        {
          file << ", " << hist[c][x];
        }
        file << std::endl;
      }
    }
#endif

  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc,char *argv[])
{

  int err = 0;
  EMMPM_Data* data = EMMPM_CreateDataStructure();
  EMMPM_CallbackFunctions* callbacks = EMMPM_AllocateCallbackFunctionStructure();
#if 1
  /* Parse the command line arguments */
  EMMPMInputParser parser;
  err = parser.parseCLIArguments(argc, argv, data);

  if (err < 0)
  {
    printf("Error trying to parse the arguments.\n");
    return 0;
  }
#else
  char* infile = (char*)(malloc(1024));
  memset(infile, 0, 1024);
  snprintf(infile, 1024, "/Users/mjackson/Desktop/EMMPM.tif");
  char* outfile = (char*)(malloc(1024));
  memset(outfile, 0, 1024);
  snprintf(outfile, 1024, "/tmp/out.tif");
  data->input_file_name = infile;
  data->output_file_name = outfile;
  data->emIterations = 1;
  data->mpmIterations = 1;
  data->in_beta = 1.0;
  data->classes = 2;
  data->dims = 1;
  data->initType = EMMPM_Basic;
  data->simulatedAnnealing = 0;
  data->grayTable[0] = 0;
  data->grayTable[1] = 255;
  data->verbose = 1;
  data->w_gamma[0] = 1.0;
  data->w_gamma[1] = 1.0;
  data->useCurvaturePenalty = 0;
  data->ccostLoopDelay = 0;
  data->beta_e = 1.0;
  data->beta_c = 0.0;
  data->r_max = 1.0;
#endif


  /* Set the Callback functions to provide feedback */
  callbacks->EMMPM_ProgressFunc = &EMMPM_PrintfProgress;
  callbacks->EMMPM_ProgressStatsFunc = &UpdateStats;

  // Get our input image from the Image IO functions
  err = EMMPM_ReadInputImage(data, callbacks);
  if (err < 0)
  {
    printf("Error Reading the input image.\n");
    return 0;
  }

  // Set the initialization function based on the command line arguments
  switch(data->initType)
  {
    case EMMPM_Basic:
      callbacks->EMMPM_InitializationFunc = EMMPM_BasicInitialization;
      break;
    case EMMPM_UserInitArea:
      callbacks->EMMPM_InitializationFunc = EMMPM_UserDefinedAreasInitialization;
      break;
    case EMMPM_ManualInit:
      callbacks->EMMPM_InitializationFunc = EMMPM_ManualInitialization;
      break;
    default:
      break;
  }

  std::cout << "EM/MPM Starting.... " << std::endl;

  // Allocate all the memory here

  err = EMMPM_AllocateDataStructureMemory(data);
  if (err)
  {
    printf("Error allocating memory for the EMMPM Data Structure.\n   %s(%d)\n", __FILE__, __LINE__);
    return 1;
  }


  EMMPM_Run(data, callbacks);


  err = EMMPM_WriteOutputImage(data, callbacks);
  if (err < 0)
  {
    return 0;
  }

  EMMPM_FreeDataStructure(data);
  EMMPM_FreeCallbackFunctionStructure(callbacks);

  std::cout << "EM/MPM Ending" << std::endl;

	return 1;
}

