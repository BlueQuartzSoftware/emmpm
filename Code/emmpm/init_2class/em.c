/* em.c */

/** usage:  em beta infile outfile mpm-iter em-iter #_of_classes **/

/* Modified by Joel Dumke on 9/4/06 */
/* Modified by Khalid Tahboub on 9/13/10 */


#include <math.h>
#include <stdlib.h>
#include "tiff.h"
#include "emmpm/common/allocate.h"
#include "emmpm/common/random.h"

#include "emmpm/common/EMTiffIO.h"

#define MAX_CLASSES 15


void mpm(unsigned char **, unsigned char **, double ***, double, double *, double *, double *, int, int, int, int);

void entropy(double ***probs, unsigned char **output, unsigned int rows, unsigned int cols, int classes);

void blur(double **h, unsigned char **in, unsigned char **out, int rows, int cols, int mask_size);

int main(int argc,char *argv[]) {
  TIFF* input_img;
  TIFF* output_img;
	FILE *fp;
	unsigned int i, j, rows, cols, x11, x12, y11, y12, x21, x22, y21, y22;
	double beta, gamma[MAX_CLASSES], ga, x, m[MAX_CLASSES], v[MAX_CLASSES], N[MAX_CLASSES];
	int enable_blur = 0;
	/*	m[l] - estimate of mean for class l
		v[l] - estimate of variance for class l
		N[l] - Used for normalization 
		(x11,y11) & (x12,y12) specify the rectangle coodrinates to initialize the first class
		(x21,y21) & (x22,y22) specify the rectangle coodrinates to initialize the second class
	*/
size_t index;
int err;
	

	int l, mpmiter, emiter, k, kk, classes;
	unsigned char **y, **xt, **output;  /* output : entropy image */
	double **probs[MAX_CLASSES];

	double max_entropy = 0;
	double mu, sigma;

	/* Verify correct number of arguments */
	if (argc != 16) {
		printf("usage:  %s beta gamma infile outfile mpm-iter em-iter #_of_classes x11 y11 x12 y12 x21 y21 x22 y22\n", argv[0]);
		printf("(x11,y11) & (x12,y12) specify the rectangle coodrinates to initialize the first class.\n");
		printf("(x21,y21) & (x22,y22) specify the rectangle coodrinates to initialize the second class.\n");
		exit(1);
	}

	beta = atof(argv[1]);
	ga = atof(argv[2]);
	mpmiter= atoi(argv[5]);
	emiter= atoi(argv[6]);
	classes = atoi(argv[7]);

	x11	= atoi(argv[8]);
	y11	= atoi(argv[9]);
	x12	= atoi(argv[10]);
	y12	= atoi(argv[11]);
	x21	= atoi(argv[12]);
	y21	= atoi(argv[13]);
	x22	= atoi(argv[14]);
	y22	= atoi(argv[15]);



	max_entropy = log10(classes)/log(2);

	readseed();

	for(i = 0; i < MAX_CLASSES; i++)
		gamma[i] = 0;

	gamma[0] = ga;

	unsigned char* raster;

	raster = EM_ReadTiffAsGrayScale(argv[3], &cols, &rows);
	uint8_t* dst = raster;
	/* Copy input image to y[][] */
	y = (unsigned char **)get_img(cols, rows, sizeof(char));
  for (i = 0; i < rows; i++)
  {
    for (j = 0; j < cols; j++)
    {
      y[i][j] = *dst;
      ++dst;
    }
  }

  _TIFFfree( raster ); // Release the memory used to read the image
  input_img = NULL;

	xt = (unsigned char **)get_img(cols, rows, sizeof(char));


	/* Initialization of parameter estimation */

	sigma = 0;

	mu = 0;
	for (i=y11; i<y12; i++)
		for (j=x11; j<x12; j++)
			mu += y[i][j];
	mu /= (y12 - y11)*(x12 - x11);
	m[1] = mu;
	printf("m[1]=%f\n",mu);

	mu = 0;
	for (i=y21; i<y22; i++)
		for (j=x21; j<x22; j++)
			mu += y[i][j];
	mu /= (y22 - y21)*(x22 - x21);	
	m[0] = mu;
	printf("m[0]=%f\n",mu);





	

	for (l = 0; l < classes; l++) {
		v[l] = 20;
		probs[l] = (double **)get_img(cols, rows, sizeof(double));
	}

	/* Initialize classification of each pixel randomly with a uniform disribution */
	for (i = 0; i < rows; i++)
		for (j = 0; j < cols; j++) {
			x = random2();
			l = 0;
			while ((double)(l + 1) / classes <= x)  // may incur l = classes when x = 1
				l++;
			xt[i][j] = l;
		}

	/* Perform EM */
	for (k = 0; k < emiter; k++) {
		/* Perform MPM */
		mpm(y, xt, probs, beta, gamma, m, v, rows, cols, mpmiter, classes);
		
		/* Reset model parameters to zero */
		for (l = 0; l < classes; l++) {
			m[l] = 0;
			v[l] = 0;
			N[l] = 0;
		}

		/*** Some efficiency was sacrificed for readability below ***/

		/* Update estimates for mean of each class */
		for (l = 0; l < classes; l++) {
			for (i = 0; i < rows; i++) {
				for (j = 0; j < cols; j++) {
					N[l] += probs[l][i][j];  // denominator of (20)
					m[l] += y[i][j] * probs[l][i][j];  // numerator of (20)
				}
			}
			if (N[l] != 0) 
				m[l] = m[l] / N[l];  // Eq. (20)
		}

		/* Update estimates of variance of each class */
		for (l = 0; l < classes; l++) {
			for (i = 0; i < rows; i++) {
				for (j = 0; j < cols; j++)
					// numerator of (21)
					v[l] += (y[i][j] - m[l]) * (y[i][j] - m[l]) * probs[l][i][j];
			}
			if (N[l] != 0) 
				v[l] = v[l] / N[l];
		}
		
		/* Monitor estimates of mean and variance */
		if (emiter < 10 || (k + 1) % (emiter / 10) == 0) {
			for (l = 0; l < classes - 1; l++)
				printf("%.3f %.3f ", m[l], v[l]);
			printf("%.3f %.3f\n", m[classes - 1], v[classes - 1]);
		}

		/* Eliminate any classes that have zero probability */
		for (kk = 0; kk < classes; kk++)
			if (N[kk] == 0) {
				for (l = kk; l < classes - 1; l++) {
					/* Move other classes to fill the gap */
					N[l] = N[l + 1];
					m[l] = m[l + 1];
					v[l] = v[l + 1];
					for (i = 0; i < rows; i++)
					for (j = 0; j < cols; j++)
						if (xt[i][j] == l + 1)
							xt[i][j] = l;
				}
				classes = classes - 1;  // push the eliminated class into the last class
			}
	}


	/* Allocate space for the output image, and copy a scaled xt */

  raster = (unsigned char*)_TIFFmalloc(cols * rows);
  index = 0;
  for (i = 0; i < rows; i++)
  {
    for (j = 0; j < cols; j++)
    {
      raster[index++] = (int)xt[i][j] * 255 / (classes - 1);
    }
  }

	err = EM_WriteGrayScaleTiff(raster, argv[4], cols, rows, argv[4], "Segmented with EM/MPM");
	if (err < 0)
	{
	  printf("Error writing Tiff file %s\n", argv[4]);
	  return 0;
	}
	else
	{
	  printf("Wrote output image %s\n", argv[4]);
	}
	_TIFFfree(raster);

	free_img((void **)xt);
	free_img((void **)y);
	for (l = 0; l < classes; l++)
		free_img((void **)probs[l]);

	writeseed();

	return 0;
}

