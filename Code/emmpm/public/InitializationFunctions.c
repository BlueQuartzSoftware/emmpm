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
#include <math.h>
#include <string.h>

#include "InitializationFunctions.h"

#include "emmpm/public/EMMPM.h"
//#include "emmpm/common/allocate.h"
#include "emmpm/common/random.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_InitializeXtArray(EMMPM_Data* data)
{
  int i, l;
  size_t total;

  total = data->rows * data->columns;
  /* Initialize classification of each pixel randomly with a uniform disribution */
  for (i = 0; i < total; i++)
  {
      l = genrand_real2() * data->classes;
      data->xt[i] = l;
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_CurvatureInitialization(EMMPM_Data* data)
{
  int d;
  int i;
  int j;
  size_t ld, ijd, ij, lij, ijd1;
  int l;
  double x;

  int dims = data->dims;
  int classes = data->classes;

  size_t nsCols = data->columns - 1;
  size_t nsRows = data->rows;
  size_t ewCols = data->columns;
  size_t ewRows = data->rows - 1;
  size_t swCols = data->columns - 1;
  size_t swRows = data->rows - 1;
  size_t nwCols = data->columns-1;
  size_t nwRows = data->rows-1;

  /***  Choose initial conditions by placing means randomly
   and setting variances to 20 in each dimension (unless scalar image) ***/
  d = 0;
  for (l = 0; l < classes; l++)
  {
    if (dims == 1)
    {
      ld = dims * l + d;
      data->m[ld] = (128 + 255 * l) / classes;
      data->v[ld] = 20;
    }
    else
    {
      for (d = 0; d < data->dims; d++)
      {
        ld = dims * l + d;
        data->m[ld] = rand() % 256;
        data->v[ld] = 20;
      }
    }
  }

  /* Initialize classification of each pixel randomly with a uniform disribution */
  //EMMPM_InitializeXtArray(data);
  for (i = 0; i < data->rows; i++)
  {
    for (j = 0; j < data->columns; j++)
    {
      x = genrand_real2();
      l = 0;
      while ((double)(l + 1) / classes <= x)
      { l++; }
      ij = (data->columns*i) + j;
      data->xt[ij] = l;
    }
  }

  /* Allocate for edge images */

  data->ns = (double*)malloc(nsCols * nsRows * sizeof(double));
  data->ew = (double*)malloc(ewCols * ewRows * sizeof(double));
  data->sw = (double*)malloc(swCols * swRows * sizeof(double));
  data->nw = (double*)malloc(nwCols * nwRows * sizeof(double));
  data->ccost = (double*)malloc(data->classes * data->rows * data->columns * sizeof(double));


  /* Do edge detection */
  for (i = 0; i < data->rows; i++)
  {
    for (j = 0; j < nwCols; j++)
    {
      x = 0;
      for (d = 0; d < dims; d++)
      {
        ijd = (dims * nwCols * i) + ( dims * j) + d;
        ijd1 = (dims * nwCols * (i)) + (dims * (j+1)) + d;
        x += (data->y[ijd] - data->y[ijd1]) * (data->y[ijd] - data->y[ijd1]);
      }
      ij = (nwCols*i) + j;
      data->ns[ij] = data->beta_e * atan((10 - sqrt(x)) / 5);
    }
  }
  for (i = 0; i < nwRows; i++)
  {
    for (j = 0; j < data->columns; j++)
    {
      x = 0;
      for (d = 0; d < dims; d++)
      {
        ijd = (dims * data->columns * i) + ( dims * j) + d;
        ijd1 = (dims * data->columns * (i+1)) + (dims * (j)) + d;
        x += (data->y[ijd] - data->y[ijd1]) * (data->y[ijd] - data->y[ijd1]);
      }
      ij = (data->columns*i) + j;
      data->ew[ij] = data->beta_e * atan((10 - sqrt(x)) / 5);
    }
  }
  nwCols = data->columns-1;
  nwRows = data->rows-1;
  for (i = 0; i < nwRows; i++)
  {
    for (j = 0; j < nwCols; j++)
    {
      x = 0;
      for (d = 0; d < data->dims; d++)
      {
        ijd = (dims * data->columns * i) + ( dims * j) + d;
        ijd1 = (dims * data->columns * (i+1)) + ( dims * (j+1)) + d;
        x += (data->y[ijd] - data->y[ijd1]) * (data->y[ijd] - data->y[ijd1]);
      }
      ij = (nwCols*i) + j;
      data->sw[ij] = data->beta_e * atan((10 - sqrt(0.5 * x)) / 5);
      x = 0;
      for (d = 0; d < data->dims; d++)
      {
        ijd = (dims * data->columns * (i+1)) + ( dims * (j)) + d;
        ijd1 = (dims * data->columns * (i)) + ( dims * (j+1)) + d;
        x += (data->y[ijd] - data->y[ijd1]) * (data->y[ijd] - data->y[ijd1]);
      }
      ij = (nwCols*i) + j;
      data->nw[ij] = data->beta_e * atan((10 - sqrt(0.5 * x)) / 5);
    }
  }

  /* Initialize Curve Costs to zero */
  for (l = 0; l < data->classes; l++)
  {
    for (i = 0; i < data->rows; i++)
    {
      for (j = 0; j < data->columns; j++)
      {
        {
          lij = (data->columns*data->rows*l) + (data->columns*i) + j;
          data->ccost[lij] = 0;
        }
      }
    }
  }



}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_BasicInitialization(EMMPM_Data* data)
{

  //FIXME: This needs to be adapted for vector images (dims > 1)
  unsigned int i, k, l;
  double mu, sigma;
  char msgbuff[256];
  unsigned int rows = data->rows;
  unsigned int cols = data->columns;
  unsigned int classes = data->classes;
//  double rndNum = 0.0;
  unsigned char* y = data->y;
//  unsigned char** xt = data->xt;
  size_t total;

  rows = data->rows;
  cols = data->columns;
  total = data->rows * data->columns;


//  printf("- EMMPM_BasicInitialization()\n");
  memset(msgbuff, 0, 256);

  //  printf("EMMPM_BasicInitialization Starting\n");
  //  if (callbacks->EMMPM_ProgressFunc != NULL) {
  //    callbacks->EMMPM_ProgressFunc("EMMPM_BasicInitialization Starting", data->progress);
  //  }
  /* Initialization of parameter estimation */
  mu = 0;
  sigma = 0;
  for (i = 0; i < total; i++) {
      mu += y[i];
  }

  mu /= (rows * cols);

  for (i = 0; i < total; i++) {
      sigma += (y[i] - mu) * (y[i] - mu);
  }

  sigma /= (rows * cols);
  sigma = sqrt((double)sigma);
  // printf("mu=%f sigma=%f\n",mu,sigma);
  // printf("- mu=%f sigma=%f\n", mu, sigma);

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
//    data->probs[l] = (double **)get_img(cols, rows, sizeof(double));
  }
  EMMPM_InitializeXtArray(data);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_UserDefinedAreasInitialization(EMMPM_Data* data)
{
  unsigned int i, j;
  size_t index;
  int c, l;
  double mu, sigma;
  unsigned int rows = data->rows;
  unsigned int cols = data->columns;
  char msgbuff[256];
  unsigned char* y = data->y;
//  unsigned char** xt = data->xt;
  rows = data->rows;
  cols = data->columns;

  sigma = 0;
  mu = 0;

  if (data->dims != 1)
  {
    printf("User Defined Initialization ONLY works with GrayScale images and not vector images.\n  %s(%d)", __FILE__, __LINE__);
    exit(1);
  }

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
        index = (cols * i) + j;
        mu += y[index];
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
     // data->probs[l] = (double **)get_img(data->columns, data->rows, sizeof(double));
    }
    else
    {
      data->v[l] = -1;
     // data->probs[l] = NULL;
    }
  }

  EMMPM_InitializeXtArray(data);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_ManualInitialization(EMMPM_Data* data)
{
  EMMPM_InitializeXtArray(data);
}
