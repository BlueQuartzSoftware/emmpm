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
#include <string.h>

#include "InitializationFunctions.h"

#include "emmpm/public/EMMPM.h"
#include "emmpm/common/utilities/allocate.h"
#include "emmpm/common/utilities/random.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_InitializeXtArray(EMMPM_Data* data)
{
  int i, j, l;
  double rndNum;
  /* Initialize classification of each pixel randomly with a uniform disribution */
  for (i = 0; i < data->rows; i++)
  {
    for (j = 0; j < data->columns; j++)
    {
      l = genrand_real2() * data->classes;
      data->xt[i][j] = l;
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_BasicInitialization(EMMPM_Data* data)
{
  unsigned int i, j, k, l;
  double mu, sigma;
  char msgbuff[256];
  unsigned int rows = data->rows;
  unsigned int cols = data->columns;
  unsigned int classes = data->classes;
//  double rndNum = 0.0;
  unsigned char** y = data->y;
//  unsigned char** xt = data->xt;

  printf("- EMMPM_BasicInitialization()\n");

  rows = data->rows;
  cols = data->columns;

  memset(msgbuff, 0, 256);

  //  printf("EMMPM_BasicInitialization Starting\n");
  //  if (callbacks->EMMPM_ProgressFunc != NULL) {
  //    callbacks->EMMPM_ProgressFunc("EMMPM_BasicInitialization Starting", data->progress);
  //  }
  /* Initialization of parameter estimation */
  mu = 0;
  sigma = 0;
  for (i = 0; i < rows; i++)
    for (j = 0; j < cols; j++)
      mu += y[i][j];

  mu /= (rows * cols);

  for (i = 0; i < rows; i++)
    for (j = 0; j < cols; j++)
      sigma += (y[i][j] - mu) * (y[i][j] - mu);
  sigma /= (rows * cols);
  sigma = sqrt((double)sigma);
  //printf("mu=%f sigma=%f\n",mu,sigma);
  printf("- mu=%f sigma=%f\n", mu, sigma);

  //  if (callbacks->EMMPM_ProgressFunc != NULL) {
  //    callbacks->EMMPM_ProgressFunc(msgbuff, data->progress);
  //  }


  if (classes % 2 == 0)
  {
    for (k = 0; k < classes / 2; k++)
    {
      data->m[classes / 2 + k] = mu + (k + 1) * sigma / 2;
      data->m[classes / 2 - 1 - k] = mu - (k + 1) * sigma / 2;
    }
  }
  else
  {
    data->m[classes / 2] = mu;
    for (k = 0; k < classes / 2; k++)
    {
      data->m[classes / 2 + 1 + k] = mu + (k + 1) * sigma / 2;
      data->m[classes / 2 - 1 - k] = mu - (k + 1) * sigma / 2;
    }
  }

  for (l = 0; l < classes; l++)
  {
    data->v[l] = 20;
    data->probs[l] = (double **)get_img(cols, rows, sizeof(double));
  }
  EMMPM_InitializeXtArray(data);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_UserDefinedAreasInitialization(EMMPM_Data* data)
{
 // char startMsg[] = "InitNClassInitialization Starting";
//  EMMPM_ShowProgress(startMsg, 1.0f);
//  printf("EMMPM_UserDefinedAreasInitialization Starting\n");
  unsigned int i, j;
  int c, l;
  double mu, sigma;
  unsigned int rows = data->rows;
  unsigned int cols = data->columns;
  char msgbuff[256];
  unsigned char** y = data->y;
//  unsigned char** xt = data->xt;
  rows = data->rows;
  cols = data->columns;

  sigma = 0;
  mu = 0;


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
 //   EMMPM_ShowProgress(msgbuff, 1.0);
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
 //   EMMPM_ShowProgress(msgbuff, 1.0);
  }

  for (l = 0; l < EMMPM_MAX_CLASSES; l++) {
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

  EMMPM_InitializeXtArray(data);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_ManualInitialization(EMMPM_Data* data)
{
//  unsigned int i, j;
  int l;
//  unsigned char** xt = data->xt;
 // double rndNum;

  for (l = 0; l < EMMPM_MAX_CLASSES; l++) {
    if (l < data->classes) {
      //data->v[l] = 20;
      data->probs[l] = (double **)get_img(data->columns, data->rows, sizeof(double));
    }
    else
    {
      data->v[l] = -1;
      data->probs[l] = NULL;
    }
  }
  EMMPM_InitializeXtArray(data);
}
