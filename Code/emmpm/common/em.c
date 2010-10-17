/*
 * em.c
 *
 *  Created on: Oct 17, 2010
 *      Author: mjackson
 */


#include "em.h"


void EM_PerformEMLoops(EMMPM_Inputs* inputs,
                       unsigned char **y,
                       unsigned char **xt,
                       double **probs[MAX_CLASSES],
                       double gamma[MAX_CLASSES],
                       double m[MAX_CLASSES],
                       double v[MAX_CLASSES],
                       double N[MAX_CLASSES])
{

  int i, j, k, l, kk;
  int emiter = inputs->emIterations;
  int rows = inputs->rows;
  int cols = inputs->columns;
  int mpmiter = inputs->mpmIterations;
  int classes = inputs->classes;
  double beta = inputs->beta;

  /* Perform EM Loops*/
  for (k = 0; k < emiter; k++)
  {
    /* Perform MPM */
    mpm(y, xt, probs, beta, gamma, m, v, rows, cols, mpmiter, classes);

    /* Reset model parameters to zero */
    for (l = 0; l < classes; l++)
    {
      m[l] = 0;
      v[l] = 0;
      N[l] = 0;
    }
    /*** Some efficiency was sacrificed for readability below ***/
    /* Update estimates for mean of each class */
    for (l = 0; l < classes; l++)
    {
      for (i = 0; i < rows; i++)
      {
        for (j = 0; j < cols; j++)
        {
          N[l] += probs[l][i][j]; // denominator of (20)
          m[l] += y[i][j] * probs[l][i][j]; // numerator of (20)
        }
      }

      if (N[l] != 0) m[l] = m[l] / N[l];

      // Eq. (20)}
      /* Update estimates of variance of each class */
      for (l = 0; l < classes; l++)
      {
        for (i = 0; i < rows; i++)
        {
          for (j = 0; j < cols; j++)
            // numerator of (21)
            v[l] += (y[i][j] - m[l]) * (y[i][j] - m[l]) * probs[l][i][j];

        }
        if (N[l] != 0) v[l] = v[l] / N[l];

      }
      /* Monitor estimates of mean and variance */
      if (emiter < 10 || (k + 1) % (emiter / 10) == 0)
      {
        for (l = 0; l < classes - 1; l++) {
          printf("%.3f %.3f ", m[l], v[l]);
        }

        printf("%.3f %.3f\n", m[classes - 1], v[classes - 1]);
      }
      /* Eliminate any classes that have zero probability */
      for (kk = 0; kk < classes; kk++)
      {
        if (N[kk] == 0)
        {
          for (l = kk; l < classes - 1; l++)
          {
            /* Move other classes to fill the gap */
            N[l] = N[l + 1];
            m[l] = m[l + 1];
            v[l] = v[l + 1];
            for (i = 0; i < rows; i++)
              for (j = 0; j < cols; j++)
                if (xt[i][j] == l + 1) xt[i][j] = l;

          }
          classes = classes - 1; // push the eliminated class into the last class
        }
      }
    }
  }
}
