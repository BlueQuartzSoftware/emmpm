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
#include <math.h>

#include <iostream>
#include <ostream>
#include <fstream>
#include <string>

#include "emmpm/common/EMMPMTypes.h"
#include "emmpm/public/EMMPM_Structures.h"
#include "emmpm/public/EMMPM.h"
#include "emmpm/public/EMMPMInputParser.h"
#include "emmpm/tiff/EMTiffIO.h"
#include "emmpm/public/ProgressFunctions.h"
#include "emmpm/public/InitializationFunctions.h"

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
    snprintf(buff, 256, "/tmp/curvature_out_%d.tif", data->currentEMLoop);
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
    snprintf(buff, 256, "/tmp/curvature_hist_%d.csv", data->currentEMLoop);
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
  }

}

//
//acem 1.5 1.5 0 ../../dataset/Rene-88/slice ../../output/temp/output.tiff 1 0 10 10 3
//
int main(int argc, char **argv)
{
  int err = 0;

  EMMPM_Data* data = EMMPM_CreateDataStructure();
  EMMPM_CallbackFunctions* callbacks = EMMPM_AllocateCallbackFunctionStructure();

  data->in_beta = 1.5; // arg[1]
  data->beta_e = 1.5; // arg[2]
  data->beta_c = 0.0; // arg[3]
  data->input_file_name = "/Users/Shared/Data/Rene88DT/slice000.tiff"; // arg[4]
  data->output_file_name = "/tmp/slice000_curve.tiff"; // arg[5]
  data->dims = 1; // arg[6]
  data->r_max = 0; // arg[7]
  data->mpmIterations = 10; // arg[8]
  data->emIterations = 10; // arg[9]
  data->classes = 3; // arg[10]
  data->grayTable[0] = 0;
  data->grayTable[1] = 128;
  data->grayTable[2] = 255;


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

  callbacks->EMMPM_InitializationFunc = EMMPM_CurvatureInitialization;

  std::cout << "EM/MPM Curvature Starting.... " << std::endl;

  // Allocate all the memory here

  err = EMMPM_AllocateDataStructureMemory(data);
  if (err)
  {
    printf("Error allocating memory for the EMMPM Data Structure.\n   %s(%d)\n", __FILE__, __LINE__);
    return 1;
  }

  EMMPM_CurvaturePenaltyAlgo(data, callbacks);

  err = EMMPM_WriteOutputImage(data, callbacks);
  if (err < 0)
  {
    return 0;
  }

  data->input_file_name = NULL;
  data->output_file_name = NULL;
  EMMPM_FreeDataStructure(data);
  EMMPM_FreeCallbackFunctionStructure(callbacks);

  std::cout << "EM/MPM Ending" << std::endl;
  return 0;
}
