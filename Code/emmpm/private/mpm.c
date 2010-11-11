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

#include <math.h>

#include "mpm.h"

#include "emmpm/common/utilities/allocate.h"
#include "emmpm/common/utilities/random.h"
#include "emmpm/public/EMMPM.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void mpm(EMMPM_Data* data, EMMPM_CallbackFunctions* callbacks)

{
  double local_beta;
  int classes = data->classes;
  data->progress++;
  if (callbacks->EMMPM_ProgressFunc != NULL) {
    callbacks->EMMPM_ProgressFunc("-MPM Loops", data->progress);
  }

	double **yk[MAX_CLASSES], sqrt2pi, current, con[MAX_CLASSES], d[MAX_CLASSES];
	double x, post[MAX_CLASSES], sum;
	int i, j, k, l, mm, prior[MAX_CLASSES];

  local_beta = data->workingBeta;

	/* Allocate space for yk[][][] */
	for (l = 0; l < classes; l++)
		yk[l] = (double **)get_img(data->columns, data->rows, sizeof(double));

	sqrt2pi = sqrt(2.0 * PI);

	for (l = 0; l < classes; l++) {
		con[l] = -log(sqrt2pi * sqrt(data->v[l]));
		d[l] = -2.0 * data->v[l];
	}

	for (i = 0; i < data->rows; i++)
		for (j = 0; j < data->columns; j++) {
			mm = data->y[i][j];
			for (l = 0; l < classes; l++) {
				data->probs[l][i][j] = 0;  // reset content of (16)
				yk[l][i][j] = con[l] + ((mm - data->m[l]) * (mm - data->m[l]) / d[l]);
			}
		}

	for (k = 0; k < data->mpmIterations; k++)
		for (i=0; i<data->rows; i++)
			for (j=0; j<data->columns; j++) {
				sum = 0;
				for (l = 0; l < classes; l++) {
					prior[l] = 0;
					if (i - 1 >= 0) {
						if (j - 1 >= 0)
							if (data->xt[i - 1][j - 1] != l)
	 							(prior[l])++;
						if (data->xt[i - 1][j] != l)
							(prior[l])++;
						if (j + 1 < data->columns)
							if (data->xt[i - 1][j + 1] != l)
								(prior[l])++;
					}
					if (i + 1 < data->rows) {
						if (j - 1 >= 0)
							if (data->xt[i + 1][j - 1] != l)
								(prior[l])++;
						if (data->xt[i + 1][j] != l)
							(prior[l])++;
						if (j + 1 < data->columns)
							if (data->xt[i + 1][j + 1] != l)
								(prior[l])++;
					}
					if (j - 1 >= 0)
						if (data->xt[i][j - 1] != l)
							(prior[l])++;
					if (j + 1 < data->columns)
						if (data->xt[i][j + 1] != l)
							(prior[l])++;

					post[l] = exp(yk[l][i][j] - local_beta * (double)(prior[l]) - data->w_gamma[l]);
					sum += post[l];
				}
				x = random2();
				current = 0;

				for (l = 0; l < classes; l++) {
					if ((x >= current) && (x <= (current + post[l] / sum))) {
						data->xt[i][j] = l;
						data->probs[l][i][j] += 1.0;
					}
					current += post[l] / sum;
				}
			}

	/* Normalize probabilities */
	for (i=0; i<data->rows; i++)
		for (j=0; j<data->columns; j++)
			for (l = 0; l < classes; l++)
				data->probs[l][i][j] = data->probs[l][i][j] / (double)data->mpmIterations;

	/* Clean Up */
	for (l = 0; l < classes; l++)
	  EMMPM_free_img((void **)yk[l]);

}
