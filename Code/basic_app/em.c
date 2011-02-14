/* em.c */

/** usage:  em beta infile outfile mpm-iter em-iter #_of_classes **/

/* Modified by Joel Dumke on 9/4/06 */


#include <stdlib.h>
#include "tiff.h"
#include "allocate.h"
#include "random.h"
#define EMMPM_MAX_CLASSES 15


void mpm(unsigned char **, unsigned char **, double ***, double, double *, double *, double *, int, int, int, int);

void entropy(double ***probs, unsigned char **output, unsigned int rows, unsigned int cols, int classes);

void blur(double **h, unsigned char **in, unsigned char **out, int rows, int cols, int mask_size);

int main(int argc,char *argv[]) {
	struct TIFF_img input_img, output_img;
	FILE *fp;
	unsigned int i, j, rows, cols;
	double beta, gamma[EMMPM_MAX_CLASSES], ga, x, m[EMMPM_MAX_CLASSES], v[EMMPM_MAX_CLASSES], N[EMMPM_MAX_CLASSES];
	int enable_blur = 0;
	/*	m[l] - estimate of mean for class l
		v[l] - estimate of variance for class l
		N[l] - Used for normalization 
	*/
	int l, mpmiter, emiter, k, kk, classes;
	unsigned char **y, **xt, **output;  /* output : entropy image */
	double **probs[EMMPM_MAX_CLASSES];

	double max_entropy = 0;
	double mu, sigma;

	/* Verify correct number of arguments */
	if (argc != 8) {
		printf("usage:  %s beta gamma infile outfile mpm-iter em-iter #_of_classes\n", argv[0]);
		exit(1);
	}

	beta = atof(argv[1]);
	ga = atof(argv[2]);
	mpmiter= atoi(argv[5]);
	emiter= atoi(argv[6]);
	classes = atoi(argv[7]);

	max_entropy = log10(classes)/log(2);

	readseed();

	for(i = 0; i < EMMPM_MAX_CLASSES; i++)
		gamma[i] = 0;

	gamma[0] = ga;

	/* open image file */
	if ((fp = fopen(argv[3], "rb")) == NULL) {
		printf("Cannot open file %s\n", argv[3]);
		exit(1);
	}

	/* read image */
	if (read_TIFF(fp, &input_img)) {
		printf("Error reading file %s\n", argv[3]);
		exit(1);
	}

	/* close image file */
	fclose(fp);

	/* check the type of image data */
	if (input_img.TIFF_type != 'g') {
		printf("Error:  Image must be grayscale.\n");
		exit(1);
	}

	cols = input_img.width;
	rows = input_img.height;

	/* Copy input image to y[][] */
	y = (unsigned char **)get_img(cols, rows, sizeof(char));
	for (i = 0; i < rows; i++)
		for (j = 0; j < cols; j++)
			y[i][j] = input_img.mono[i][j];
	free_TIFF(&input_img);

	xt = (unsigned char **)get_img(cols, rows, sizeof(char));


	/* Initialization of parameter estimation */
	mu = 0;
	sigma = 0;
	for (i=0; i<rows; i++)
		for (j=0; j<cols; j++)
			mu += y[i][j];
	mu /= rows*cols;

	for (i=0; i<rows; i++)
		for (j=0; j<cols; j++)
			sigma += (y[i][j]-mu)*(y[i][j]-mu);
	sigma /= rows*cols;
	sigma = sqrt((double)sigma);
	printf("mu=%f sigma=%f\n",mu,sigma);

	if (classes%2 == 0)
	{
		for (k=0; k<classes/2; k++)
		{
			m[classes/2 + k] = mu + (k+1)*sigma/2;
			m[classes/2 - 1 - k] = mu - (k+1)*sigma/2;
		}
	}
	else
	{
		m[classes/2] = mu;
		for (k=0; k<classes/2; k++)
		{
			m[classes/2 + 1 + k] = mu + (k+1)*sigma/2;
			m[classes/2 - 1 - k] = mu - (k+1)*sigma/2;
		}
	}

	for (l = 0; l < classes; l++) {
		v[l] = 20;
		probs[l] = (double **)get_img(cols, rows, sizeof(double));
	}

	/* Initialize classification of each pixel randomly with a uniform disribution */
	for (i = 0; i < rows; i++)
		for (j = 0; j < cols; j++) {
			x = genrand_real2();
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
	get_TIFF(&output_img, rows, cols, 'g');

	for (i=0; i<rows; i++)
		for (j=0; j<cols; j++)
			output_img.mono[i][j] = (int)xt[i][j] * 255 / (classes - 1);

	/* open image file */
	if ((fp = fopen(argv[4], "wb")) == NULL ) {
		printf("Cannot open file %s\n", argv[4]);
		exit(1);
	}

	/* write image */
	if (write_TIFF(fp, &output_img)) {
		printf("Error writing TIFF file %s\n", argv[4]);
		exit(1);
	}

	/* close image file */
	fclose(fp);

	/* Clean up */
	free_TIFF(&output_img);

	free_img((void **)xt);
	free_img((void **)y);
	for (l = 0; l < classes; l++)
		free_img((void **)probs[l]);

	writeseed();

	return 0;
}

