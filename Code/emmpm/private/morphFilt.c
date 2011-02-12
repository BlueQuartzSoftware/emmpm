/* morphFilt.c */

/* Written by Joel Dumke on 1/30/09 */

/* This function calculates the curvature cost values stored in curve based on
	the classification image, xt, morphologically opened under structuring
	element se */

/* Note: This is the function where I really got windowing right for the first time. */

#include <stddef.h>
#include <stdlib.h>
#include <math.h>

#include <emmpm/private/morphFilt.h>


#define NUM_SES 8

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int maxi(int a, int b) {
	if (a < b)
		return (unsigned int)b;
	return (unsigned int)a;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int mini(int a, int b) {
	if (a < b)
		return (unsigned int)a;
	return (unsigned int)b;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void morphFilt(EMMPM_Data* data, unsigned char* curve, unsigned char* se, int r)
{
  unsigned char* erosion;
  unsigned int i, j, l, maxr, maxc;
  int ii, jj;
  size_t ij, i1j1, iirjjr;

  int rows = data->rows;
  int cols = data->columns;
  int classes = data->classes;

  erosion = (unsigned char*)malloc(cols * rows * sizeof(unsigned char));

  for (i = 0; i < rows; i++)
  {
    for (j = 0; j < cols; j++)
    {
      ij = (cols * i) + j;

      curve[ij] = classes;
      l = data->xt[ij];
      erosion[ij] = l;
      maxr = mini(r, rows - 1 - i);
      maxc = mini(r, cols - 1 - j);
      for (ii = -mini(r, i); ii <= (int)maxr; ii++)
      {
        for (jj = -mini(r, j); jj <= (int)maxc && erosion[ij] == l; jj++)
        {
          i1j1 = (cols * (i+ii)) + (j+jj);
          iirjjr = (cols * (ii+r)) + (jj+r);
          if (se[iirjjr] == 1 && data->xt[i1j1] != l)
          {
            erosion[ij] = classes;
          }
        }
      }
    }
  }

  for (ii = -r; ii <= r; ii++)
  {
    for (jj = -r; jj <= r; jj++)
    {
      iirjjr = (cols * (ii+r)) + (jj+r);
      if (se[iirjjr] == 1)
      {
        maxr = rows - maxi(0, ii);
        maxc = cols - maxi(0, jj);
        for (i = maxi(0, -ii); i < maxr; i++)
          for (j = maxi(0, -jj); j < maxc; j++)
          {
            ij = (cols * i) + j;
            l = erosion[ij];
            if (l != classes) {
              i1j1 = (cols * (i+ii)) + (j+jj);
              curve[i1j1] = l;
            }
          }
      }
    }
  }

  free(erosion);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void multiSE(EMMPM_Data* data)
{
  unsigned int i, j, k, l;
  int ii, jj, ri;
  unsigned char* se = NULL;
  unsigned char* curve = NULL;
  double r, r_sq, pnlty;
  size_t ij, lij, iirijjri;
 // int dims = data->dims;
  int rows = data->rows;
  int cols = data->columns;
  int classes = data->classes;

  pnlty = 1 / (double)NUM_SES;

  curve = (unsigned char*)malloc(cols * rows * sizeof(unsigned char));

  for (k = 0; k < NUM_SES; k++)
  {
    /* Calculate new r */
    r = data->r_max / (k + 1);

    r_sq = r * r;
    ri = (int)r;

    /* Create Morphological SE */
    size_t se_cols = (2 * ri + 1);
    size_t se_rows = (2 * ri + 1);
    se = (unsigned char*)malloc(se_cols * se_rows * sizeof(unsigned char));
    for (ii = -((int)ri); ii <= (int)ri; ii++)
    {
      for (jj = -((int)ri); jj <= (int)ri; jj++)
      {
        iirijjri = (se_cols * (ii+ri)) + (jj+ri);
        if (ii * ii + jj * jj <= r_sq)
        {
          se[iirijjri] = 1;
//          se[ii + ri][jj + ri] = 1;
        }
        else
        {
          se[iirijjri] = 0;
//          se[ii + ri][jj + ri] = 0;
        }
      }
    }
    morphFilt(data, curve, se, ri);

    for (i = 0; i < rows; i++)
    {
      for (j = 0; j < cols; j++)
      {
        ij = (cols * i) + j;
        l = curve[ij];
        if (l == classes)
        {
          l = data->xt[ij];
          lij = (cols * rows * l) + (cols * i) + j;
          data->ccost[lij] += pnlty;
        }
      }
    }

    free(se);

  }

  free(curve);
}
