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
#error
#include "emmpm/common/EMMPMTypes.h"
#include "emmpm/public/EMMPM_Constants.h"
#include "emmpm/public/EMMPM_Structures.h"

#include "curvature_em.h"
#include "morphFilt.h"

#define CURVE_DELAY 10


void EMMPM_CurvatureEMLoops(EMMPM_Data* data, EMMPM_CallbackFunctions* callbacks)
{
  size_t l, d, i, j, k, kk;


  /* Perform EM */
   for (k = 0; k < data->emIterations; k++)
   {
     /* After CURVE_DELAY iterations, begin calculating curvature costs */
     if (k >= CURVE_DELAY) {
       multiSE(data);
     }

     /* Perform MPM - (Estimation) */
     acvmpm(yvec, ns, ew, sw, nw, xt, ccost, probs, beta, beta_c, m, v, rows, columns, mpmiter, classes, dims);

     /* Reset model parameters to zero */
     for (l = 0; l < data->classes; l++)
     {
       for (d = 0; d < data->dims; d++)
       {
         data->m[l][d] = 0;
         data->v[l][d] = 0;
       }
       data->N[l] = 0;
     }

     /*** Some efficiency was sacrificed for readability below ***/

     /* Update estimates for mean of each class - (Maximization) */
     for (l = 0; l < data->classes; l++)
     {
       for (i = 0; i < data->rows; i++)
         for (j = 0; j < data->columns; j++)
         {
           data->N[l] += data->probs[l][i][j];
           for (d = 0; d < data->dims; d++)
             data->m[l][d] += data->yvec[i][j][d] * data->probs[l][i][j];
         }
       if (data->N[l] != 0) for (d = 0; d < data->dims; d++)
         data->m[l][d] = data->m[l][d] / data->N[l];
     }

     /* Update estimates of variance of each class */
     for (l = 0; l < data->classes; l++)
     {
       for (i = 0; i < data->rows; i++)
         for (j = 0; j < data->columns; j++)
           for (d = 0; d < data->dims; d++)
             data->v[l][d] += (data->yvec[i][j][d] - data->m[l][d]) * (data->yvec[i][j][d] - data->m[l][d]) * data->probs[l][i][j];
       if (data->N[l] != 0) for (d = 0; d < data->dims; d++)
         data->v[l][d] = data->v[l][d] / data->N[l];
     }

     /* Monitor estimates of mean and variance */
     if (data->emIterations < 10 || (k + 1) % (data->emIterations / 10) == 0)
     {
       for (l = 0; l < data->classes; l++)
       {
         printf(" *** Class %d:\n", l + 1);
         printf("      m:  ");
         for (d = 0; d < data->dims; d++)
           printf("%.3f ", data->m[l][d]);
         printf("\n      v:  ");
         for (d = 0; d < data->dims; d++)
           printf("%.3f ", data->v[l][d]);
         printf("\n\n");
       }
       printf(" **************************\n\n\n");
     }

     /* Eliminate any classes that have zero probability */
     for (kk = 0; kk < data->classes; kk++)
       if (data->N[kk] == 0)
       {
         for (l = kk; l < data->classes - 1; l++)
         {
           /* Move other classes to fill the gap */
           data->N[l] = data->N[l + 1];
           for (d = 0; d < data->dims; d++)
           {
             data->m[l][d] = data->m[l + 1][d];
             data->v[l][d] = data->v[l + 1][d];
           }
         }
         for (i = 0; i < data->rows; i++)
           for (j = 0; j < data->columns; j++)
             if (data->xt[i][j] > kk) data->xt[i][j]--;
         data->classes = data->classes - 1;
       }
   }

}
