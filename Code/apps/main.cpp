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
void InitNClassInitialization(EMMPM_Inputs* inputs, EMMPM_WorkingVars* vars)
{
  char startMsg[] = "InitNClassInitialization Starting";
  EMMPM_ShowProgress(startMsg, 1.0f);

  unsigned int i, j;
  int c, l;
  double mu, sigma;
  unsigned int rows = inputs->rows;
  unsigned int cols = inputs->columns;

  unsigned char** y = vars->y;
  unsigned char** xt = vars->xt;
  rows = inputs->rows;
  cols = inputs->columns;

  sigma = 0;
  mu = 0;

  char msgbuff[256];
  memset(msgbuff, 0, 256);

  for(c = 0; c < inputs->classes; c++)
  {
 //   printf("####################################################################\n");
    int x1 = inputs->initCoords[c][0];
    int y1 = inputs->initCoords[c][1];
    int x2 = inputs->initCoords[c][2];
    int y2 = inputs->initCoords[c][3];
    mu = 0;
    snprintf(msgbuff, 256, "m[%d] Coords: %d %d %d %d", c, x1, y1, x2, y2);
    EMMPM_ShowProgress(msgbuff, 1.0);
    for (i=inputs->initCoords[c][1]; i<inputs->initCoords[c][3]; i++) {
      for (j=inputs->initCoords[c][0]; j<inputs->initCoords[c][2]; j++) {
        mu += y[i][j];
   //     printf ("%03d ", y[i][j]);
      }
   //   printf("\n");
    }

    mu /= (y2 - y1)*(x2 - x1);
    vars->m[c] = mu;
    snprintf(msgbuff, 256, "m[%d]=%f", c, mu);
    EMMPM_ShowProgress(msgbuff, 1.0);
  }

  for (l = 0; l < MAX_CLASSES; l++) {
    if (l < inputs->classes) {
      vars->v[l] = 20;
      vars->probs[l] = (double **)get_img(inputs->columns, inputs->rows, sizeof(double));
    }
    else
    {
      vars->v[l] = -1;
      vars->probs[l] = NULL;
    }
  }

  /* Initialize classification of each pixel randomly with a uniform disribution */
  for (i = 0; i < inputs->rows; i++) {
    for (j = 0; j < inputs->columns; j++) {
      vars->x = random2();
      l = 0;
      while ((double)(l + 1) / inputs->classes <= vars->x)  // may incur l = classes when x = 1
        l++;
      xt[i][j] = l;
    }
  }
  char endMsg[] = "InitNClassInitialization Complete";
  EMMPM_ShowProgress(endMsg , 4.0f);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UpdateStats(EMMPM_Update* update)
{
  std::cout << "EM Loop: " << update->currentEMLoop << std::endl;
  char buff[256];
  memset(buff, 0, 256);
  snprintf(buff, 256, "/tmp/emmpm_out_%d.tif", update->currentEMLoop);
  int err = writeGrayScaleImage(buff, update->height, update->width, "Intermediate Image", update->outputImage);
  if (err < 0)
  {
    std::cout << "Error writing intermediate tiff image." << std::endl;
  }

  std::cout << "Class\tMu\tSigma" << std::endl;
  for (int l = 0; l < update->classes; l++)
  {
//    snprintf(msgbuff, 256, "%d\t%.3f\t%.3f", l, update->m[l], update->v[l]);
//    EMMPM_ShowProgress(msgbuff, vars->progress);
    std::cout << l << "\t" << update->m[l] << "\t" << update->v[l] << "\t" << std::endl;
  }

  float hist[MAX_CLASSES][256];
  // Generate a gaussian curve for each class based off the mu and sigma for that class
  for (int c = 0; c < update->classes; ++c)
  {
    float mu = update->m[c];
    float sig = update->v[c];
    float twoSigSqrd = sig * sig * 2.0f;
    float constant = 1.0 / (sig * sqrtf(2.0f * M_PI));
    for (size_t x = 0; x < 256; ++x)
    {
      hist[c][x] = constant * exp(-1.0f * ((x - mu) * (x - mu)) / (twoSigSqrd));
    }
  }

  memset(buff, 0, 256);
  snprintf(buff, 256, "/tmp/emmpm_hist_%d.csv", update->currentEMLoop);
  std::ofstream file(buff, std::ios::out | std::ios::binary);
  if (file.is_open())
  {
    for (size_t x = 0; x < 256; ++x)
    {
      file << x;
      for (int c = 0; c < update->classes; ++c)
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

  EMMPM_Files* files = EMMPM_AllocateFilesStructure();
  EMMPM_Inputs* inputs = EMMPM_AllocateInputsStructure();

  int err = parser.parseCLIArguments(argc, argv, files, inputs);
  if (err < 0)
  {
    printf("Error trying to parse the arguments.\n");
    return 0;
  }

  // Get our input image from the Image IO functions
  err = EMMPM_ReadInputImage(files, inputs);
  if (err < 0)
  {
    printf("Error Reading the input image.\n");
    return 0;
  }

  // Set the initialization function to the Basic
  EMMPM_SetInitializationFunction( &EMMPM_BasicInitialization);
  if (inputs->initType == EMMPM_PIXEL_AREA_INITIALIZATION)
  {
    EMMPM_SetInitializationFunction( &InitNClassInitialization);
  }

  EMMPM_SetProgressStatsFunction( &UpdateStats);

  // Run the EM/MPM algorithm on the input image
  EMMPM_Execute(files, inputs);

  err = EMMPM_WriteOutputImage(files, inputs);
  if (err < 0)
  {
    return 0;
  }

  EMMPM_FreeTiffImageBuffer( files->inputImage ); // Release the memory used to read the image
  EMMPM_FreeTiffImageBuffer( files->outputImage );

  EMMPM_FreeFilesStructure(files);
  EMMPM_FreeInputsStructure(inputs);

  std::cout << "EM/MPM Ending" << std::endl;
	return 1;
}

