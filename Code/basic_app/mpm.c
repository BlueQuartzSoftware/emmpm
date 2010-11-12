/* mpm.c */

/* Modified by Joel Dumke on 9/4/06 */

#include <math.h>
#include "allocate.h"
#include "random.h"
#define PI  3.14159265358979323846
#define MAX_CLASSES 15

void mpm(unsigned char **y, unsigned char **xt, double **pr[], double beta, double *gamma, double *m, double *v, int rows, int cols, int n, int classes) {
	double **yk[MAX_CLASSES], sqrt2pi, current, con[MAX_CLASSES], d[MAX_CLASSES];
	double x, post[MAX_CLASSES], sum;
	int i, j, k, l, mm, prior[MAX_CLASSES];

	/* Allocate space for yk[][][] */
	for (l = 0; l < classes; l++)
		yk[l] = (double **)get_img(cols, rows, sizeof(double));

	sqrt2pi = sqrt(2.0 * PI);

	for (l = 0; l < classes; l++) {
		con[l] = -log(sqrt2pi * sqrt(v[l]));
		d[l] = -2.0 * v[l];
	}

	for (i = 0; i < rows; i++)
		for (j = 0; j < cols; j++) {
			mm = y[i][j];
			for (l = 0; l < classes; l++) {
				pr[l][i][j] = 0;  // reset content of (16)
				yk[l][i][j] = con[l] + ((mm - m[l]) * (mm - m[l]) / d[l]);
			}
		}

	for (k = 0; k < n; k++)
		for (i=0; i<rows; i++)
			for (j=0; j<cols; j++) {
				sum = 0;
				for (l = 0; l < classes; l++) {
					prior[l] = 0;
					if (i - 1 >= 0) {
						if (j - 1 >= 0)
							if (xt[i - 1][j - 1] != l)
	 							(prior[l])++;
						if (xt[i - 1][j] != l)  
							(prior[l])++;
						if (j + 1 < cols)
							if (xt[i - 1][j + 1] != l) 
								(prior[l])++;
					}
					if (i + 1 < rows) {
						if (j - 1 >= 0)
							if (xt[i + 1][j - 1] != l) 
								(prior[l])++;
						if (xt[i + 1][j] != l) 
							(prior[l])++;
						if (j + 1 < cols)
							if (xt[i + 1][j + 1] != l) 
								(prior[l])++;
					}
					if (j - 1 >= 0)
						if (xt[i][j - 1] != l) 
							(prior[l])++;
					if (j + 1 < cols)
						if (xt[i][j + 1] != l) 
							(prior[l])++;

					post[l] = exp(yk[l][i][j] - beta * (double)(prior[l]) - gamma[l]);
					sum += post[l];
				}
				x = random2();
				current = 0;
				
				for (l = 0; l < classes; l++) {
					if ((x >= current) && (x <= (current + post[l] / sum))) {
						xt[i][j] = l;
						pr[l][i][j] += 1.0;
					}
					current += post[l] / sum;
				}
			}

	/* Normalize probabilities */
	for (i=0; i<rows; i++)
		for (j=0; j<cols; j++)
			for (l = 0; l < classes; l++)
				pr[l][i][j] = pr[l][i][j] / (double)n;

	/* Clean Up */
	for (l = 0; l < classes; l++)
		free_img((void **)yk[l]);

}
