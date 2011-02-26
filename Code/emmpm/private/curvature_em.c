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
#include <stdio.h>

#include <string.h>
#include <stdlib.h>

#include "emmpm/common/EMMPMTypes.h"
#include "emmpm/public/EMMPM_Constants.h"
#include "emmpm/public/EMMPM_Structures.h"
#include "emmpm/common/EMMPM_Math.h"
#include "emmpm/public/InitializationFunctions.h"
#include "emmpm/private/em.h"

#include "curvature_em.h"
#include "morphFilt.h"
#include "curvature_mpm.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_CurvatureEMLoops(EMMPM_Data* data, EMMPM_CallbackFunctions* callbacks)
{
  size_t i, k;
  int emiter = data->emIterations;
  double* simAnnealBetas = NULL;

  float totalLoops = (float)(data->emIterations * data->mpmIterations);
  float currentLoopCount = 0.0;

  size_t ccostLoopDelay = data->ccostLoopDelay;

  char msgbuff[256];
  memset(msgbuff, 0, 256);
  data->currentEMLoop = 0;
  data->currentMPMLoop = 0;

  data->workingBeta = data->in_beta;

  if (data->simulatedAnnealing != 0)
  {
    simAnnealBetas=(double*)(malloc(sizeof(double)*data->emIterations));
    for (i = 0; i < data->emIterations; ++i)
    {
      simAnnealBetas[i] = data->in_beta + pow(i/(data->emIterations-1.0), 8) * (10.0*data->in_beta - data->in_beta);
    }
    data->workingBeta = simAnnealBetas[0];
  }

  /* Perform EM Loops*/
  for (k = 0; k < emiter; k++)
  {
    if (data->cancel) { data->progress = 100.0; break; }

    data->inside_em_loop = 1;
    data->currentEMLoop = k;
    data->currentMPMLoop = 0;
    currentLoopCount = data->mpmIterations * data->currentEMLoop + data->currentMPMLoop;

    data->progress = currentLoopCount/totalLoops * 100.0;
    if (callbacks->EMMPM_ProgressFunc != NULL) {
      snprintf(msgbuff, 256, "EM Loop %d", data->currentEMLoop);
      callbacks->EMMPM_ProgressFunc(msgbuff, data->progress);
    }

    // Possibly update the beta value due to simulated Annealing
    if (data->simulatedAnnealing)  {
      data->workingBeta = simAnnealBetas[k];
    }

    /* After curveLoopDelay iterations, begin calculating curvature costs */
    if (k >= ccostLoopDelay)
    {
      multiSE(data);
    }

    /* Perform MPM - (Estimation) */
    //acvmpm(y, ns, ew, sw, nw, xt, ccost, probs, beta, beta_c, m, v, rows, columns, mpmiter, classes, dims);
    acvmpm(data, callbacks);

    /* Reset model parameters to zero */
    EMMPM_ResetModelParameters(data);

    /* Update Means and Variances */
    EMMPM_UpdateMeansAndVariances(data);

#if 0
    /* Monitor estimates of mean and variance */
    if (emiter < 10 || (k + 1) % (emiter / 10) == 0)
    {
      EMMPM_MonitorMeansAndVariances(data, callbacks);
    }
#endif

    EMMPM_ConvertXtToOutputImage(data, callbacks);

#if 1
    /* Eliminate any classes that have zero probability */
    EMMPM_RemoveZeroProbClasses(data);
#endif

    if (NULL != callbacks->EMMPM_ProgressStatsFunc)
    {
      callbacks->EMMPM_ProgressStatsFunc(data);
    }
  } /* EM Loop End */

  data->inside_em_loop = 0;
  free(simAnnealBetas);
}
