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


#include "emmpm/public/EMMPM_Structures.h"
#include "emmpm/public/EMMPM.h"
#include "emmpm/common/utilities/allocate.h"
#include "emmpm/common/utilities/random.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_BasicInitialization(EMMPM_Inputs* inputs, EMMPM_WorkingVars* vars)
{
  unsigned int i, j, k, l;
  double mu, sigma;
  unsigned int rows = inputs->rows;
  unsigned int cols = inputs->columns;
  unsigned int classes = inputs->classes;

  unsigned char** y = vars->y;
  unsigned char** xt = vars->xt;
  rows = inputs->rows;
  cols = inputs->columns;
  char msgbuff[256];
  memset(msgbuff, 0, 256);

  EMMPM_ShowProgress("EMMPM_BasicInitialization Starting", vars->progress);
  /* Initialization of parameter estimation */
  mu = 0;
  sigma = 0;
  for (i = 0; i < rows; i++)
    for (j = 0; j < cols; j++)
      mu += y[i][j];

  mu /= rows * cols;

  for (i = 0; i < rows; i++)
    for (j = 0; j < cols; j++)
      sigma += (y[i][j] - mu) * (y[i][j] - mu);
  sigma /= rows * cols;
  sigma = sqrt((double)sigma);
  //printf("mu=%f sigma=%f\n",mu,sigma);
  snprintf(msgbuff, 256, "mu=%f sigma=%f\n", mu, sigma);
  EMMPM_ShowProgress(msgbuff, vars->progress);

  if (classes%2 == 0)
  {
    for (k=0; k<classes/2; k++)
    {
      vars->m[classes/2 + k] = mu + (k+1)*sigma/2;
      vars->m[classes/2 - 1 - k] = mu - (k+1)*sigma/2;
    }
  }
  else
  {
    vars->m[classes/2] = mu;
    for (k=0; k<classes/2; k++)
    {
      vars->m[classes/2 + 1 + k] = mu + (k+1)*sigma/2;
      vars->m[classes/2 - 1 - k] = mu - (k+1)*sigma/2;
    }
  }

  for (l = 0; l < classes; l++)
  {
    vars->v[l] = 20;
    vars->probs[l] = (double **)get_img(cols, rows, sizeof(double));
  }

  /* Initialize classification of each pixel randomly with a uniform disribution */
  for (i = 0; i < rows; i++)
  for (j = 0; j < cols; j++)
  {
    vars->x = random2();
    l = 0;
    while ((double)(l + 1) / classes <= vars->x) // may incur l = classes when x = 1
    l++;
    xt[i][j] = l;
  }


}
