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
#include "emmpm/common/utilities/allocate.h"
#include "emmpm/common/utilities/random.h"
#include "emmpm/public/EMMPM.h"
#include "emmpm/public/EMMPMInputParser.h"
#include "emmpm/common/io/EMTiffIO.h"
#include "emmpm/common/utilities/ProgressFunctions.h"
#include "emmpm/common/utilities/InitializationFunctions.h"



/**
 * @brief
 * @param inputs
 * @param vars
 */
void InitNClassInitialization(EMMPM_Data* data)
{
//  char startMsg[] = "InitNClassInitialization Starting";
//  if (callbacks->EMMPM_ProgressFunc != NULL) {
//    callbacks->EMMPM_ProgressFunc(startMsg, 1.0);
//  }

  unsigned int i, j;
  int c, l;
  double mu, sigma;
  unsigned int rows = data->rows;
  unsigned int cols = data->columns;

  unsigned char** y = data->y;
  unsigned char** xt = data->xt;
  rows = data->rows;
  cols = data->columns;

  sigma = 0;
  mu = 0;

  char msgbuff[256];
  memset(msgbuff, 0, 256);

  for(c = 0; c < data->classes; c++)
  {
 //   printf("####################################################################\n");
    int x1 = data->initCoords[c][0];
    int y1 = data->initCoords[c][1];
    int x2 = data->initCoords[c][2];
    int y2 = data->initCoords[c][3];
    mu = 0;
    snprintf(msgbuff, 256, "m[%d] Coords: %d %d %d %d", c, x1, y1, x2, y2);
//    if (callbacks->EMMPM_ProgressFunc != NULL) {
//      callbacks->EMMPM_ProgressFunc(msgbuff, 1.0);
//    }

    for (i=data->initCoords[c][1]; i<data->initCoords[c][3]; i++) {
      for (j=data->initCoords[c][0]; j<data->initCoords[c][2]; j++) {
        mu += y[i][j];
   //     printf ("%03d ", y[i][j]);
      }
   //   printf("\n");
    }

    mu /= (y2 - y1)*(x2 - x1);
    data->m[c] = mu;
    snprintf(msgbuff, 256, "m[%d]=%f", c, mu);
//    if (callbacks->EMMPM_ProgressFunc != NULL) {
//      callbacks->EMMPM_ProgressFunc(msgbuff, 1.0);
//    }
  }

  for (l = 0; l < MAX_CLASSES; l++) {
    if (l < data->classes) {
      data->v[l] = 20;
      data->probs[l] = (double **)get_img(data->columns, data->rows, sizeof(double));
    }
    else
    {
      data->v[l] = -1;
      data->probs[l] = NULL;
    }
  }

  /* Initialize classification of each pixel randomly with a uniform disribution */
  for (i = 0; i < data->rows; i++) {
    for (j = 0; j < data->columns; j++) {
      data->x = random2();
      l = 0;
      while ((double)(l + 1) / data->classes <= data->x)  // may incur l = classes when x = 1
        l++;
      xt[i][j] = l;
    }
  }
//  char endMsg[] = "InitNClassInitialization Complete";
//  if (callbacks->EMMPM_ProgressFunc != NULL) {
//    callbacks->EMMPM_ProgressFunc(endMsg, 4.0);
//  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateStats(EMMPM_Data* data)
{
  std::cout << "EM Loop: " << data->currentEMLoop << std::endl;
  char buff[256];
  memset(buff, 0, 256);
  snprintf(buff, 256, "/tmp/emmpm_out_%d.tif", data->currentEMLoop);
  int err = writeGrayScaleImage(buff, data->rows, data->columns, "Intermediate Image", data->outputImage);
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

  float hist[MAX_CLASSES][256];
  // Generate a gaussian curve for each class based off the mu and sigma for that class
  for (int c = 0; c < data->classes; ++c)
  {
    float mu = data->m[c];
    float sig = data->v[c];
    float twoSigSqrd = sig * sig * 2.0f;
    float constant = 1.0 / (sig * sqrtf(2.0f * M_PI));
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


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc,char *argv[])
{
  std::cout << "emmpm Starting.... " << std::endl;
  EMMPMInputParser parser;

  EMMPM_Data* data = EMMPM_AllocateDataStructure();
  EMMPM_CallbackFunctions* callbacks = EMMPM_AllocateCallbackFunctionStructure();

  int err = parser.parseCLIArguments(argc, argv, data);
  if (err < 0)
  {
    printf("Error trying to parse the arguments.\n");
    return 0;
  }

  // Get our input image from the Image IO functions
  err = EMMPM_ReadInputImage(data, callbacks);
  if (err < 0)
  {
    printf("Error Reading the input image.\n");
    return 0;
  }

  // Set the initialization function to the Basic
  switch(data->initType)
  {
    case EMMPM_PIXEL_AREA_INITIALIZATION:
      callbacks->EMMPM_InitializationFunc = InitNClassInitialization;
      break;
    case EMMPM_BASIC_INITIALIZATION:
      callbacks->EMMPM_InitializationFunc = EMMPM_BasicInitialization;
      break;
    default:
      break;
  }

  callbacks->EMMPM_ProgressStatsFunc = &UpdateStats;

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

