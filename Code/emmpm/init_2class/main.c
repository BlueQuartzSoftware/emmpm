/* em.c */

/** usage:  em beta infile outfile mpm-iter em-iter #_of_classes **/

/* Modified by Joel Dumke on 9/4/06 */
/* Modified by Khalid Tahboub on 9/13/10 */



#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "emmpm/common/EMMPMTypes.h"
#include "emmpm/common/EMMPM_Constants.h"
#include "emmpm/common/em.h"
#include "emmpm/common/entropy.h"
#include "emmpm/common/allocate.h"
#include "emmpm/common/random.h"
#include "emmpm/common/EMTiffIO.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc,char *argv[])
{


	unsigned int i, j;
	unsigned int x11, x12, y11, y12, x21, x22, y21, y22;
	double gamma[MAX_CLASSES], x, m[MAX_CLASSES], v[MAX_CLASSES], N[MAX_CLASSES];
//	int enable_blur = 0;
	/*	m[l] - estimate of mean for class l
		v[l] - estimate of variance for class l
		N[l] - Used for normalization
		(x11,y11) & (x12,y12) specify the rectangle coodrinates to initialize the first class
		(x21,y21) & (x22,y22) specify the rectangle coodrinates to initialize the second class
	*/
  int err;


	int l;
	unsigned char **y, **xt;
//	unsigned char **output;  /* output : entropy image */
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
/*
	beta = atof(argv[1]);
	ga = atof(argv[2]);
	mpmiter= atoi(argv[5]);
	emiter= atoi(argv[6]);
	classes = atoi(argv[7]);
*/

	/* Parse the inputs from the command line */
  EMMPM_Inputs inputs;
  inputs.beta = atof(argv[1]);
  inputs.gamma = atof(argv[2]);
  inputs.mpmIterations = atoi(argv[5]);
  inputs.emIterations = atoi(argv[6]);
  inputs.classes = atoi(argv[7]);
  inputs.input_file = argv[3];
  inputs.output_file = argv[4];

  /* Parse extended arguments */
	x11	= atoi(argv[8]);
	y11	= atoi(argv[9]);
	x12	= atoi(argv[10]);
	y12	= atoi(argv[11]);
	x21	= atoi(argv[12]);
	y21	= atoi(argv[13]);
	x22	= atoi(argv[14]);
	y22	= atoi(argv[15]);

	max_entropy = log10(inputs.classes)/log(2);

	readseed();

	for(i = 0; i < MAX_CLASSES; i++) {
		gamma[i] = 0;
	}
	gamma[0] = inputs.gamma;


	/* Get our input data from a Tiff Formatted Image */
	y = EM_ReadInputImage(inputs.input_file, &(inputs.columns), &(inputs.rows));

	/* Allocate memory for the xt arrays */
	xt = (unsigned char **)get_img(inputs.columns, inputs.rows, sizeof(char));

	//TODO: Need to create an initialization callback function and assign it to the EMMPM library
	//TODO: Look at the SHP Encoder code for an example on how to do this

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

	for (l = 0; l < inputs.classes; l++) {
		v[l] = 20;
		probs[l] = (double **)get_img(inputs.columns, inputs.rows, sizeof(double));
	}

	/* Initialize classification of each pixel randomly with a uniform disribution */
	for (i = 0; i < inputs.rows; i++) {
		for (j = 0; j < inputs.columns; j++) {
			x = random2();
			l = 0;
			while ((double)(l + 1) / inputs.classes <= x)  // may incur l = classes when x = 1
				l++;
			xt[i][j] = l;
		}
	}

	/* Run the EM Loops */
  EM_PerformEMLoops(&inputs, y, xt, probs,  gamma, m, v,  N);

  /* Allocate space for the output image, and copy a scaled xt
	 * and then write the output image.*/
	err = EM_WriteOutputImage(inputs.output_file, inputs.columns, inputs.rows, inputs.classes, xt);
	if (err < 0)
	{
	  return 0;
	}


	free_img((void **)xt);
	free_img((void **)y);
	for (l = 0; l < inputs.classes; l++)
		free_img((void **)probs[l]);

	writeseed();

	return 0;
}

