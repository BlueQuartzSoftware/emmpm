/* em.c */

/** usage:  em beta infile outfile mpm-iter em-iter num_of_classes **/

/* Modified by Joel Dumke on 9/4/06 */
/* Modified by Khalid Tahboub on 9/13/10 */

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
#include "emmpm/common/io/EMTiffIO.h"
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
    snprintf(buff, 256, "/tmp/emmpm_out_%d.tif", data->currentEMLoop);
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
      double constant = 1.0f / (sig * sqrtf(2.0f * PI));
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
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc,char *argv[])
{


  EMMPM_Data* data = EMMPM_AllocateDataStructure();
  EMMPM_CallbackFunctions* callbacks = EMMPM_AllocateCallbackFunctionStructure();

  /* Parse the comand line arguments */
  EMMPMInputParser parser;
  int err = parser.parseCLIArguments(argc, argv, data);
  if (err < 0)
  {
    printf("Error trying to parse the arguments.\n");
    return 0;
  }

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
    case EMMPM_USER_DEFINED_AREA_INITIALIZATION:
      callbacks->EMMPM_InitializationFunc = EMMPM_UserDefinedAreasInitialization;
      break;
    case EMMPM_BASIC_INITIALIZATION:
      callbacks->EMMPM_InitializationFunc = EMMPM_BasicInitialization;
      break;
    default:
      break;
  }

  std::cout << "emmpm Starting.... " << std::endl;

  // Run the EM/MPM algorithm on the input image
  EMMPM_Execute(data, callbacks);

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

