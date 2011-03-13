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

#include "emmpm/common/EMMPM_Math.h"
#include "emmpm/common/random.h"
#include "emmpm/public/EMMPM.h"
#include "emmpm/private/mpm.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void mpm(EMMPM_Data* data, EMMPM_CallbackFunctions* callbacks)

{
  double* yk;
  double sqrt2pi, current, con[EMMPM_MAX_CLASSES];
  double x, post[EMMPM_MAX_CLASSES], sum;
  int k, l, prior;
  int i, j, d[EMMPM_MAX_CLASSES];
  size_t ij, lij, i1j1;
//  int dims = data->dims;
  int rows = data->rows;
  int cols = data->columns;
  int classes = data->classes;
  unsigned char* y = data->y;
  unsigned char* xt = data->xt;
  double* probs = data->probs;
  double* m = data->m;
  double* v = data->v;
//  double* ccost = data->ccost;
//  double* ns = data->ns;
//  double* ew = data->ew;
//  double* sw = data->sw;
//  double* nw = data->nw;
  char msgbuff[256];
  float totalLoops;
  float currentLoopCount = 0.0;
  double local_beta;

  double mm;

  local_beta = data->in_beta;

  memset(post, 0, EMMPM_MAX_CLASSES * sizeof(double));
  memset(con, 0,  EMMPM_MAX_CLASSES * sizeof(double));

  totalLoops = (float)(data->emIterations * data->mpmIterations);
  memset(msgbuff, 0, 256);
  data->progress++;

  yk = (double*)malloc(cols * rows * classes * sizeof(double));

  sqrt2pi = sqrt(2.0 * M_PI);

  for (l = 0; l < classes; l++)
  {
    con[l] = -log(sqrt2pi * sqrt(v[l]));
    d[l] = -2.0 * v[l];
  }

  for (i = 0; i < rows; i++)
  {
    for (j = 0; j < cols; j++)
    {
      ij = (cols * i) + j;
      mm = y[ij];
      for (l = 0; l < classes; l++)
      {
        lij = (cols * rows * l) + (cols * i) + j;
        probs[lij] = 0; // reset content of (16)
        yk[lij] = con[l] + ((mm - m[l]) * (mm - m[l]) / d[l]);
      }
    }
  }

  /* Perform the MPM loops */
  for (k = 0; k < data->mpmIterations; k++)
  {
    if (data->cancel) { data->progress = 100.0; break; }
    data->inside_mpm_loop = 1;
    if (callbacks->EMMPM_ProgressFunc != NULL)
    {
      data->currentMPMLoop = k;
      snprintf(msgbuff, 256, "MPM Loop %d", data->currentMPMLoop);
      callbacks->EMMPM_ProgressFunc(msgbuff, data->progress);
    }

    for (i = 0; i < rows; i++)
    {
      for (j = 0; j < cols; j++)
      {
        ij = (cols * i) + j;
        sum = 0;
        for (l = 0; l < classes; l++)
        {
          prior = 0;
          if (i - 1 >= 0)
          {
            if (j - 1 >= 0)
            {
              i1j1 = (cols*(i-1))+j-1;
              if (xt[i1j1] != l)
              {
                prior++;
              }
            }

            //mark1
            i1j1 = (cols*(i-1))+j;
            if (xt[i1j1] != l)
            {
              prior++;
            }
            //mark2
            if (j + 1 < cols)
            {
              i1j1 = (cols*(i-1))+j+1;
              if (xt[i1j1] != l)
              {
                prior++;
              }
            }
          }

          //mark3
          if (i + 1 < rows)
          {
            if (j - 1 >= 0)
            {
              i1j1 = (cols*(i+1))+j-1;
              if (xt[i1j1] != l)
              {
                prior++;
              }
            }
            //mark4
            i1j1 = (cols*(i+1))+j;
            if (xt[i1j1] != l)
            {
              prior++;
            }
            //mark5
            if (j + 1 < cols)
            {
              i1j1 = (cols*(i+1))+j+1;
              if (xt[i1j1] != l)
              {
                prior++;
              }
            }
          }
          //mark6
          if (j - 1 >= 0)
          {
            i1j1 = (cols*(i))+j-1;
            if (xt[i1j1] != l)
            {
              prior++;
            }
          }
          //mark7
          if (j + 1 < cols)
          {
            i1j1 = (cols*(i))+j+1;
            if (xt[i1j1] != l)
            {
              prior++;
            }
          }
          lij = (cols * rows * l) + (cols * i) + j;
          post[l] = exp(yk[lij] - local_beta * (double)(prior) - data->w_gamma[l]);
          sum += post[l];
        }
        x = genrand_real2(data->rngVars);
        current = 0;
        for (l = 0; l < classes; l++)
        {
          lij = (cols * rows * l) + (cols * i) + j;
          ij = (cols*i)+j;
          if ((x >= current) && (x <= (current + post[l] / sum)))
          {
            xt[ij] = l;
            probs[lij] += 1.0;
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

  if (!data->cancel)
  {
  /* Normalize probabilities */
    for (i = 0; i < rows; i++)
    {
      for (j = 0; j < cols; j++)
      {
        for (l = 0; l < classes; l++)
        {
          lij = (cols * rows * l) + (cols * i) + j;
          probs[lij] = probs[lij] / (double)data->mpmIterations;
        }
      }
    }
  }
  /* Clean up the memory */
  free(yk);
}
