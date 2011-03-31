
#include <stdio.h>

void entropy(double ***probs, unsigned char **output, unsigned int rows, unsigned int cols, unsigned int classes)
{
	unsigned int l, i, j;
	double entr;

	for(i = 0; i < rows; i++)
	{
		for(j = 0; j < cols; j++)
		{
			entr = 0;
			/* Compute entropy of each pixel */
			for(l = 0; l < classes; l++)
			{
				if(probs[l][i][j] > 0)
					entr -= probs[l][i][j] * (log10(probs[l][i][j]) / log10(2));
			}
			output[i][j] = (unsigned char)(entr + 0.5);
		}
	}

}
