/* morphFilt.c */

/* Written by Joel Dumke on 1/30/09 */

/* This function calculates the curvature cost values stored in curve based on
	the classification image, xt, morphologically opened under structuring
	element se */

/* Note: This is the function where I really got windowing right for the first time. */

#include <math.h>

#include "emmpm/public/EMMPM_Constants.h"
#include "emmpm/common/allocate.h"


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
void morphFilt(unsigned char **xt, unsigned char **curve, unsigned char** se, int r, unsigned int rows, unsigned int cols, int classes) {
	unsigned char **erosion;
	unsigned int i, j, l, maxr, maxc;
	int ii, jj;

	erosion = (unsigned char**)get_img(cols, rows, sizeof(unsigned char));

	for (i = 0; i < rows; i++)
		for (j = 0; j < cols; j++) {
			curve[i][j] = classes;
			l = xt[i][j];
			erosion[i][j] = l;
			maxr = mini(r, rows - 1 - i);
			maxc = mini(r, cols - 1 - j);
			for (ii = -mini(r, i); ii <= (int)maxr; ii++)
				for (jj = -mini(r, j); jj <= (int)maxc && erosion[i][j] == l; jj++) {
					if (se[ii + r][jj + r] == 1 && xt[i + ii][j + jj] != l)
						erosion[i][j] = classes;
				}
		}



	for (ii = -r; ii <= r; ii++)
		for (jj = -r; jj <= r; jj++)
			if (se[ii + r][jj + r] == 1) {
				maxr = rows - maxi(0, ii);
				maxc = cols - maxi(0, jj);
				for (i = maxi(0, -ii); i < maxr; i++)
					for (j = maxi(0, -jj); j < maxc; j++) {
						l = erosion[i][j];
						if (l != classes)
							curve[i + ii][j + jj] = l;
					}
			}


	free_img((void **)erosion);
}

void multiSE(unsigned char **xt, double** ccost[], double r_max, unsigned int rows, unsigned int cols, int classes) {
	unsigned int i, j, k;
	int ii, jj, ri;
	unsigned char **se, **curve;
	double r, r_sq, pnlty;

	pnlty = 1 / (double)NUM_SES;

	curve = (unsigned char**)get_img(cols, rows, sizeof(unsigned char));

	for (k = 0; k < NUM_SES; k++) {
		/* Calculate new r */
		r = r_max / (k + 1);

		r_sq = r * r;
		ri = (int)r;

		/* Create Morphological SE */
		se = (unsigned char**)get_img(2 * ri + 1, 2 * ri + 1, sizeof(unsigned char));
		for (ii = -((int)ri); ii <= (int)ri; ii++)
			for (jj = -((int)ri); jj <= (int)ri; jj++)
				if (ii * ii + jj * jj <= r_sq)
					se[ii + ri][jj + ri] = 1;
				else
					se[ii + ri][jj + ri] = 0;

		morphFilt(xt, curve, se, ri, rows, cols, classes);

		for (i = 0; i < rows; i++)
			for (j = 0; j < cols; j++)
				if (curve[i][j] == classes)
					ccost[xt[i][j]][i][j] += pnlty;


		free_img((void **)se);

	}

	free_img((void **)curve);
}
