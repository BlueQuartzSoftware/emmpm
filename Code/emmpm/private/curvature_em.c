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
#include <stddef.h>
#include <stdio.h>

#include "emmpm/common/EMMPMTypes.h"
#include "emmpm/public/EMMPM_Constants.h"
#include "emmpm/public/EMMPM_Structures.h"

#include "curvature_em.h"
#include "morphFilt.h"
#include "curvature_mpm.h"

#define CURVE_DELAY 10


void EMMPM_CurvatureEMLoops(EMMPM_Data* data, EMMPM_CallbackFunctions* callbacks)
{
  size_t l, d, i, j, k, kk;
  size_t ld, ijd, ij, lij, l1d;;
  int dims = data->dims;
  int rows = data->rows;
  int cols = data->columns;
  int classes = data->classes;

  /* Perform EM */
  for (k = 0; k < data->emIterations; k++)
  {
    /* After CURVE_DELAY iterations, begin calculating curvature costs */
    if (k >= CURVE_DELAY)
    {
      multiSE(data);
    }

    /* Perform MPM - (Estimation) */
    //acvmpm(y, ns, ew, sw, nw, xt, ccost, probs, beta, beta_c, m, v, rows, columns, mpmiter, classes, dims);
    acvmpm(data, callbacks);

    /* Reset model parameters to zero */
    for (l = 0; l < classes; l++)
    {
      for (d = 0; d < dims; d++)
      {
        ld = dims * l + d;
        data->m[ld] = 0;
        data->v[ld] = 0;
      }
      data->N[l] = 0;
    }

    /*** Some efficiency was sacrificed for readability below ***/

    /* Update estimates for mean of each class - (Maximization) */
    for (l = 0; l < classes; l++)
    {
      for (i = 0; i < rows; i++)
      {
        for (j = 0; j < cols; j++)
        {
          lij = (cols * rows * l) + (cols * i) + j;
          data->N[l] += data->probs[lij];
          for (d = 0; d < dims; d++)
          {
            ld = dims * l + d;
            ijd = (dims * cols * i) + ( dims * j) + d;
            data->m[ld] += data->y[ijd] * data->probs[lij];
          }
        }
      }
      if (data->N[l] != 0)
      {
        for (d = 0; d < dims; d++)
        {
          ld = dims * l + d;
          data->m[ld] = data->m[ld] / data->N[l];
        }
      }
    }

    /* Update estimates of variance of each class */
    for (l = 0; l < classes; l++)
    {
      for (i = 0; i < rows; i++)
      {
        for (j = 0; j < cols; j++)
        {
          lij = (cols * rows * l) + (cols * i) + j;
          for (d = 0; d < dims; d++)
          {
            ld = dims * l + d;
            ijd = (dims * cols * i) + ( dims * j) + d;
            data->v[ld] += (data->y[ijd] - data->m[ld]) * (data->y[ijd] - data->m[ld]) * data->probs[lij];
          }
        }
      }
      if (data->N[l] != 0) for (d = 0; d < dims; d++)
      {
        ld = dims * l + d;
        data->v[ld] = data->v[ld] / data->N[l];
      }
    }

    /* Monitor estimates of mean and variance */
    if (data->emIterations < 10 || (k + 1) % (data->emIterations / 10) == 0)
    {
      for (l = 0; l < classes; l++)
      {
        printf(" *** Class %ud:\n", (int)(l+1));
        printf("      m:  ");
        for (d = 0; d < dims; d++)
        {
          ld = dims * l + d;
          printf("%.3f ", data->m[ld]);
        }
        printf("\n      v:  ");
        for (d = 0; d < dims; d++)
        {
          ld = dims * l + d;
          printf("%.3f ", data->v[ld]);
        }
        printf("\n\n");
      }
      printf(" **************************\n\n\n");
    }

    /* Eliminate any classes that have zero probability */
    for (kk = 0; kk < classes; kk++)
      if (data->N[kk] == 0)
      {
        for (l = kk; l < classes - 1; l++)
        {
          /* Move other classes to fill the gap */
          data->N[l] = data->N[l + 1];
          for (d = 0; d < dims; d++)
          {
            ld = dims * l + d;
            l1d = (dims * (l+1)) + d;
            data->m[ld] = data->m[l1d];
            data->v[ld] = data->v[l1d];
          }
        }
        for (i = 0; i < rows; i++)
        {
          for (j = 0; j < cols; j++)
          {
            ij = (cols*i) + j;
            if (data->xt[ij] > kk) data->xt[ij]--;
          }
        }
        classes = classes - 1;
      }
  }

}
