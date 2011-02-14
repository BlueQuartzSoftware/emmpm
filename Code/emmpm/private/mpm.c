/* mpm.c */

/* Modified by Joel Dumke on 9/4/06 */
/*
 The Original EM/MPM algorithm was developed by Mary L. Comer and is distributed
 under the BSD License.
 Copyright (c) <2010>, <Mary L. Comer>
 All rights reserved.

 [1] Comer, Mary L., and Delp, Edward J.,  ÒThe EM/MPM Algorithm for Segmentation
 of Textured Images: Analysis and Further Experimental Results,Ó IEEE Transactions
 on Image Processing, Vol. 9, No. 10, October 2000, pp. 1731-1744.

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice, this list
 of conditions and the following disclaimer.

 Redistributions in binary form must reproduce the above copyright notice, this
 list of conditions and the following disclaimer in the documentation and/or
 other materials provided with the distribution.

 Neither the name of <Mary L. Comer> nor the names of its contributors may be
 used to endorse or promote products derived from this software without specific
 prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "emmpm/common/random.h"
#include "emmpm/public/EMMPM.h"
#include "emmpm/private/mpm.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void mpm(EMMPM_Data* data, EMMPM_CallbackFunctions* callbacks)

{
  double local_beta;
  char msgbuff[256];

  double* yk;
  double sqrt2pi, current, con[EMMPM_MAX_CLASSES], d[EMMPM_MAX_CLASSES];
  double x, post[EMMPM_MAX_CLASSES], sum;
  int i, j, k, l, mm, prior[EMMPM_MAX_CLASSES];
  int ij, lij;
  float totalLoops;
  float currentLoopCount = 0.0;
  int rows = data->rows;
  int cols = data->columns;
  int classes = data->classes;

  classes = data->classes;
  totalLoops = data->emIterations * data->mpmIterations;
  memset(msgbuff, 0, 256);
  data->progress++;

  local_beta = data->workingBeta;

  yk = (double*)malloc(cols * rows * classes * sizeof(double));

  sqrt2pi = sqrt(2.0 * M_PI);

  for (l = 0; l < classes; l++)
  {
    con[l] = -log(sqrt2pi * sqrt(data->v[l]));
    d[l] = -2.0 * data->v[l];
  }

  for (i = 0; i < data->rows; i++)
  {
    for (j = 0; j < data->columns; j++)
    {
      ij = (cols * i) + j;
      mm = data->y[ij];
      for (l = 0; l < classes; l++)
      {
        lij = (cols * rows * l) + (cols * i) + j;
        data->probs[lij] = 0; // reset content of (16)
        yk[lij] = con[l] + ((mm - data->m[l]) * (mm - data->m[l]) / d[l]);
      }
    }
  }

  /* Perform the MPM loops */
  for (k = 0; k < data->mpmIterations; k++)
  {
    data->inside_mpm_loop = 1;
    if (callbacks->EMMPM_ProgressFunc != NULL)
    {
      data->currentMPMLoop = k;
      snprintf(msgbuff, 256, "MPM Loop %d", data->currentMPMLoop);
      callbacks->EMMPM_ProgressFunc(msgbuff, data->progress);
    }
    for (i = 0; i < data->rows; i++)
    {
      for (j = 0; j < data->columns; j++)
      {
        sum = 0;
        for (l = 0; l < classes; l++)
        {
          lij = (cols * rows * l) + (cols * i) + j;

          prior[l] = 0;
          if (i - 1 >= 0)
          {
            if (j - 1 >= 0) if (data->xt[(cols*(i-1))+j- 1] != l) (prior[l])++;
            if (data->xt[(cols*(i-1))+j] != l) (prior[l])++;
            if (j + 1 < data->columns) if (data->xt[(cols*(i-1))+j+ 1] != l) (prior[l])++;
          }
          if (i + 1 < data->rows)
          {
            if (j - 1 >= 0) if (data->xt[(cols*(i+1))+j- 1] != l) (prior[l])++;
            if (data->xt[(cols*(i+1))+j] != l) (prior[l])++;
            if (j + 1 < data->columns) if (data->xt[(cols*(i+1))+j+ 1] != l) (prior[l])++;
          }
          if (j - 1 >= 0) if (data->xt[(cols*(i))+j- 1] != l) (prior[l])++;
          if (j + 1 < data->columns) if (data->xt[(cols*(i))+j+ 1] != l) (prior[l])++;

          post[l] = exp(yk[lij] - local_beta * (double)(prior[l]) - data->w_gamma[l]);
          sum += post[l];
        }
        x = genrand_real2();
        current = 0;

        for (l = 0; l < classes; l++)
        {
          lij = (cols * rows * l) + (cols * i) + j;
          ij = (cols*i)+j;
          if ((x >= current) && (x <= (current + post[l] / sum)))
          {
            data->xt[ij] = l;
            data->probs[lij] += 1.0;
          }
          current += post[l] / sum;
        }
      }
    }
    if (NULL != callbacks->EMMPM_ProgressStatsFunc)
    {
      currentLoopCount = data->mpmIterations * data->currentEMLoop + data->currentMPMLoop;
      data->progress = currentLoopCount / totalLoops * 100.0;
      callbacks->EMMPM_ProgressStatsFunc(data);
    }
  }
  data->inside_mpm_loop = 0;

  /* Normalize probabilities */
  for (i = 0; i < data->rows; i++)
  {
    for (j = 0; j < data->columns; j++)
    {
      for (l = 0; l < classes; l++)
      {
        lij = (cols * rows * l) + (cols * i) + j;
        data->probs[lij] = data->probs[lij] / (double)data->mpmIterations;
      }
    }
  }

  /* Clean Up */
//  for (l = 0; l < classes; l++)
//    EMMPM_free_img((void **)yk[l]);

  free(yk);
}
