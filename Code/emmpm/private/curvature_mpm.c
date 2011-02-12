/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted probsovided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials probsovided with the distribution.
 *
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or probsomote probsoducts derived from this software without
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

/* acvmpm.c */

/* Modified by Joel Dumke on 1/30/09 */
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "emmpm/private/curvature_mpm.h"
#include "emmpm/common/random.h"


//#include "allocate.h"
#include "emmpm/common/random.h"
//#define M_PI  3.14159265358979323846
//#define EMMPM_MAX_CLASSES 15

void acvmpm(EMMPM_Data* data, EMMPM_CallbackFunctions* callbacks)

//void acvmpm(unsigned char ***y, double **ns, double **ew, double **sw, double **nw, unsigned char **xt, double **ccost[], double **pr[], double beta, double beta_c, double **m, double **v, unsigned int rows, unsigned int cols, int n, int classes, unsigned int dim)
{
  double* yk;
  double sqrt2pi, current, con[EMMPM_MAX_CLASSES];
  double x, post[EMMPM_MAX_CLASSES], sum, edge;
  int k, l, prior;
  int i, j, d;

  size_t ld, ijd, ij, lij, i1j1;
  int dims = data->dims;
  int rows = data->rows;
  int cols = data->columns;
  int classes = data->classes;
  unsigned char* y = data->y;
  unsigned char* xt = data->xt;
  double* probs = data->probs;
  double* m = data->m;
  double* v = data->v;
  double* ccost = data->ccost;
  double* ns = data->ns;
  double* ew = data->ew;
  double* sw = data->sw;
  double* nw = data->nw;
  char msgbuff[256];
  float totalLoops;
  float currentLoopCount = 0.0;

  size_t nsCols = data->columns - 1;
  size_t nsRows = data->rows;
  size_t ewCols = data->columns;
  size_t ewRows = data->rows - 1;
  size_t swCols = data->columns - 1;
  size_t swRows = data->rows - 1;
  size_t nwCols = data->columns-1;
  size_t nwRows = data->rows-1;

  totalLoops = data->emIterations * data->mpmIterations;
  memset(msgbuff, 0, 256);
  data->progress++;
  /* Allocate space for yk[][][] */
  //	for (l = 0; l < classes; l++)
  //		yk[l] = (double **)get_img(cols, rows, sizeof(double));

  yk = (double*)malloc(cols * rows * classes * sizeof(double));

  sqrt2pi = sqrt(2.0 * M_PI);

  for (l = 0; l < classes; l++)
  {
    con[l] = 0;
    for (d = 0; d < dims; d++)
    {
      ld = dims * l + d;
      con[l] += -log(sqrt2pi * sqrt(v[ld]));
    }
  }

  for (i = 0; i < rows; i++)
  {
    for (j = 0; j < cols; j++)
    {
      for (l = 0; l < classes; l++)
      {
        lij = (cols * rows * l) + (cols * i) + j;
        probs[lij] = 0;
        yk[lij] = con[l];
        for (d = 0; d < dims; d++)
        {
          ld = dims * l + d;
          ijd = (dims * cols * i) + (dims * j) + d;
          yk[lij] += ((y[ijd] - m[ld]) * (y[ijd] - m[ld]) / (-2.0 * v[ld]));
        }
      }
    }
  }

  /* Perform the MPM loops */
  for (k = 0; k < data->mpmIterations; k++)
  {
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
          /* edge penalties (in both x and y) */
          prior = 0;
          edge = 0;
          if (i - 1 >= 0)
          {
            if (j - 1 >= 0)
            {
              i1j1 = (cols*(i-1))+j-1;
              if (xt[i1j1] != l)
              {
                prior++;
                i1j1 = (swCols*(i-1))+j-1;
                edge += sw[i1j1];
              }
            }

            //Mark1
            i1j1 = (cols*(i-1))+j;
            if (xt[i1j1] != l)
            {
              prior++;
              i1j1 = (ewCols*(i-1))+j;
              edge += ew[i1j1];
            }
            //mark2
            if (j + 1 < cols)
            {
              i1j1 = (cols*(i-1))+j+1;
              if (xt[i1j1] != l)
              {
                prior++;
                i1j1 = (nwCols*(i-1))+j;
                edge += nw[i1j1];
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
                i1j1 = (nwCols*(i))+j-1;
                edge += nw[i1j1];
              }
            }
            //mark4
            i1j1 = (cols*(i+1))+j;
            if (xt[i1j1] != l)
            {
              prior++;
              i1j1 = (ewCols*(i))+j;
              edge += ew[ij];
            }
            //mark5
            if (j + 1 < cols)
            {
              i1j1 = (cols*(i+1))+j+1;
              if (xt[i1j1] != l)
              {
                prior++;
                i1j1 = (swCols*(i))+j;
                edge += sw[ij];
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
              i1j1 = (nsCols*(i))+j-1;
              edge += ns[i1j1];
            }
          }
          //mark7
          if (j + 1 < cols)
          {
            i1j1 = (cols*(i))+j+1;
            if (xt[i1j1] != l)
            {
              prior++;
              i1j1 = (nsCols*(i))+j;
              edge += ns[ij];
            }
          }
          lij = (cols * rows * l) + (cols * i) + j;
          post[l] = exp(yk[lij] - data->in_beta * (double)prior - edge - data->beta_c * ccost[lij]);
          sum += post[l];
        }
        x = genrand_real2();
        current = 0;
        for (l = 0; l < classes; l++)
        {
          if ((x >= current) && (x <= (current + post[l] / sum)))
          {
            lij = (cols * rows * l) + (cols * i) + j;
            //ij = (cols*i) + j;
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
  /* Normalize probsobabilities */
  for (i = 0; i < rows; i++)
  {
    for (j = 0; j < cols; j++)
    {
      for (l = 0; l < classes; l++)
      {
        lij = (cols * rows * l) + (cols * i) + j;
        probs[lij] = probs[lij] / (double)(data->mpmIterations);
      }
    }
  }

  /* Clean Up */
  //	for (l = 0; l < classes; l++)
  //		free_img((unsigned char **)yk[l]);
  free(yk);

}
