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
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "em.h"
#include "mpm.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_PerformEMLoops(EMMPM_Files* files,
                          EMMPM_Inputs* inputs,
                          EMMPM_WorkingVars* vars)
{

  int i, j, k, l, kk;
  int emiter = inputs->emIterations;
  int rows = inputs->rows;
  int cols = inputs->columns;
  int classes = inputs->classes;
  char msgbuff[256];
  memset(msgbuff, 0, 256);

  float totalLoops = inputs->emIterations + 2;
  float currentLoopCount = 0.0;

  EMMPM_Update* update = EMMPM_AllocateUpdateStructure();
  update->classes = inputs->classes;
  update->width = files->width;
  update->height = files->height;
  update->channels = files->channels;
  update->currentEMLoop = 0;
  update->currentMPMLoop = 0;

  vars->workingBeta = inputs->beta;

  double* simAnnealBetas = NULL;
  if (inputs->simulatedAnnealing != 0)
  {
    simAnnealBetas=(double*)(malloc(sizeof(double)*inputs->emIterations));
    for (i = 0; i < inputs->emIterations; ++i)
    {
      simAnnealBetas[i] = inputs->beta + pow(i/(inputs->emIterations-1.0), 8) * (10.0*inputs->beta - inputs->beta);
    }
    vars->workingBeta = simAnnealBetas[0];
  }



  /* Perform EM Loops*/
  for (k = 0; k < emiter; k++)
  {
    vars->progress = currentLoopCount/totalLoops * 100.0 + 10.0;
    EMMPM_ShowProgress("EM Loop", vars->progress);
    currentLoopCount++;

    // Possibly update the beta value due to simulation Annealing
    if (inputs->simulatedAnnealing)  {
      vars->workingBeta = simAnnealBetas[k];
    }

    /* Perform MPM */
    mpm(inputs, vars);

    /* Reset model parameters to zero */
    for (l = 0; l < classes; l++)
    {
      vars->m[l] = 0;
      vars->v[l] = 0;
      vars->N[l] = 0;
    }
    /*** Some efficiency was sacrificed for readability below ***/
    /* Update estimates for mean of each class */
    for (l = 0; l < classes; l++)
    {
      for (i = 0; i < rows; i++)
      {
        for (j = 0; j < cols; j++)
        {
          vars->N[l] += vars->probs[l][i][j]; // denominator of (20)
          vars->m[l] += vars->y[i][j] * vars->probs[l][i][j]; // numerator of (20)
        }
      }
      if (vars->N[l] != 0) vars->m[l] = vars->m[l] / vars->N[l];
    }

      // Eq. (20)}
      /* Update estimates of variance of each class */
      for (l = 0; l < classes; l++)
      {
        for (i = 0; i < rows; i++)
        {
          for (j = 0; j < cols; j++)
            // numerator of (21)
            vars->v[l] += (vars->y[i][j] - vars->m[l]) * (vars->y[i][j] - vars->m[l]) * vars->probs[l][i][j];

        }
        if (vars->N[l] != 0) vars->v[l] = vars->v[l] / vars->N[l];
      }

#if 0
      /* Monitor estimates of mean and variance */
      if (emiter < 10 || (k + 1) % (emiter / 10) == 0)
      {
        for (l = 0; l < classes - 1; l++)
        {
          snprintf(msgbuff, 256, "%d\t%.3f\t%.3f", l, vars->m[l], vars->v[l]);
          EMMPM_ShowProgress(msgbuff, vars->progress);
        }
        snprintf(msgbuff, 256, "%d\t%.3f\t%.3f", (classes-1), vars->m[classes - 1], vars->v[classes - 1]);
        EMMPM_ShowProgress(msgbuff, vars->progress);
      }
#endif


      // Copy the mean and variance into the update structure;
      memcpy(update->m, vars->m, MAX_CLASSES * sizeof(double));
      memcpy(update->v, vars->v, MAX_CLASSES * sizeof(double));
      EMMPM_ConvertXtToOutputImage(files, inputs, vars);
      update->outputImage = files->outputImage;

#if 0
      /* Eliminate any classes that have zero probability */
      for (kk = 0; kk < classes; kk++)
      {
        if (vars->N[kk] == 0)
        {
          for (l = kk; l < classes - 1; l++)
          {
            /* Move other classes to fill the gap */
            vars->N[l] = vars->N[l + 1];
            vars->m[l] = vars->m[l + 1];
            vars->v[l] = vars->v[l + 1];
            for (i = 0; i < rows; i++)
            for (j = 0; j < cols; j++)
            if (vars->xt[i][j] == l + 1) vars->xt[i][j] = l;

          }
          classes = classes - 1; // push the eliminated class into the last class
        }
      }
#endif


    //TODO: Add in update functions
    EMMPM_ProgressStats(update);
    update->currentEMLoop++;

  }
  EMMPM_FreeUpdateStructure(update);
  free(simAnnealBetas);
}
