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


#include "EMMPMLib/EMMPMLibTypes.h"
#include "EMMPMLib/public/EMMPM_Constants.h"
#include "EMMPMLib/public/EMMPM_Data.h"
#include "EMMPMLib/common/EMMPM_Math.h"
#include "EMMPMLib/public/InitializationFunctions.h"
#include "EMMPMLib/private/EMMPMUtilities.h"
#include "EMMPMLib/tiff/EMTiffIO.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMUtilities::ConvertInputImageToWorkingImage(EMMPM_Data::Pointer data)
{
  uint8_t* dst;
  int i;
  int j;
  int d;
  size_t index = 0;
  int width;
  int height;
  int dims;

  if (data->inputImageChannels == 0)
  {
    printf("The number of input color channels was 0\n. Exiting Program.\n");
    return;
  }

  if (data->inputImageChannels != data->dims)
  {
    printf("The number of image channels does not match the number of vector dimensions\nExiting Program.\n");
    return;
  }


  /* Copy input image to y[][] */
  width = data->columns;
  height = data->rows;
  dims = data->dims;
  dst = data->inputImage;


  for (j = 0; j < height; j++)
  {
    for (i = 0; i < width; i++)
    {
      for (d = 0; d < dims; d++)
      {
        index = (j*width*dims) + (i*dims) + d;
        data->y[index] = *dst;
        ++dst;
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMUtilities::ConvertXtToOutputImage(EMMPM_Data::Pointer data)
{
  size_t index;
  unsigned int i, j, d;
  unsigned char* raster;
  int l, ld;
  size_t gtindex = 0;
  size_t* classCounts = NULL;
  size_t x = 0;

  real_t mu = 0.0;
  real_t stdDev = 0.0;
  real_t twoSigSqrd = 0.0f;
  real_t constant = 0.0;
  real_t variance = 0.0;

  float sqrt2pi = sqrtf(2.0f * (float)M_PI);
  size_t histIdx = 0;
  float pixelWeight = 0.0;
  size_t totalPixels = 0;

  // Initialize all the counts to Zero
  classCounts = (size_t*)malloc(data->classes * sizeof(size_t));
  memset(classCounts, 0, data->classes * sizeof(size_t));

  if (data->outputImage == NULL)
  {
    TiffUtilities tifUtil;
    data->outputImage = tifUtil.allocateTiffImageBuffer(data->columns, data->rows, data->dims);
  }
  raster = data->outputImage;
  index = 0;
  totalPixels = data->rows * data->columns;
  for (i = 0; i < data->rows; i++)
  {
    for (j = 0; j < data->columns; j++)
    {
      gtindex = data->xt[ i*data->columns + j ];
      classCounts[gtindex]++;
      raster[index++] = data->grayTable[gtindex];
    }
  }
  // Now we have the counts for the number of pixels of each class.
  // The "classes" loop could be its own threaded Task at this point
  //printf("=============================\n");
  for (d = 0; d < data->dims; d++){
    for (l = 0; l < data->classes; ++l)
    {
      pixelWeight = (float)(classCounts[l])/(float)(totalPixels);
      ld = data->dims * l + d;
      mu = data->m[ld];
      variance = data->v[ld];
      stdDev = sqrt( data->v[ld] ); // Standard Deviation is the Square Root of the Variance
      twoSigSqrd = variance * 2.0f; // variance is StdDev Squared, so just use the Variance value
      constant = 1.0f / (stdDev * sqrt2pi);
      //printf("Class %d: Sigma %f  Peak Height: %f\n", l, sig, (constant * pixelWeight));
      for (x = 0; x < 256; ++x)
      {
        histIdx = (256*data->classes*d) + (256*l) + x;
        data->histograms[histIdx] = pixelWeight * constant * exp(-1.0f * ((x - mu) * (x - mu)) / (twoSigSqrd));
      }
    }
  }


  free(classCounts);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMUtilities::ResetModelParameters(EMMPM_Data::Pointer dt)
{
  EMMPM_Data* data = dt.get();

  size_t l, d, ld;
  /* Reset model parameters to zero */
  for (l = 0; l < data->classes; l++)
  {
    for (d = 0; d < data->dims; d++)
    {
      ld = data->dims * l + d;
      data->m[ld] = 0;
      data->v[ld] = 0;
    }
    data->N[l] = 0;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMUtilities::UpdateMeansAndVariances(EMMPM_Data::Pointer dt)
{
  EMMPM_Data* data = dt.get();

  size_t l, i, j, d, ld, ijd, lij;
  size_t dims = data->dims;
  size_t rows = data->rows;
  size_t cols = data->columns;
  size_t classes = data->classes;

  /*** Some efficiency was sacrificed for readability below ***/
  /* Update estimates for mean of each class - (Maximization) */
  for (l = 0; l < classes; l++)
  {
    for (i = 0; i < rows; i++)
    {
      for (j = 0; j < cols; j++)
      {
        lij = (cols * rows * l) + (cols * i) + j;
        data->N[l] += data->probs[lij]; // denominator of (20)
        for (d = 0; d < dims; d++)
        {
          ld = dims * l + d;
          ijd = (dims * cols * i) + ( dims * j) + d;
          data->m[ld] += data->y[ijd] * data->probs[lij]; // numerator of (20)
        }
      }
    }
    if (data->N[l] != 0)
    {
      for (d = 0; d < dims; d++)
      {
        ld = dims * l + d;
        data->m[ld] = data->m[ld] / data->N[l];
      }
    }
  }

  // Eq. (20)}
  /* Update estimates of variance of each class */
  for (l = 0; l < classes; l++)
  {
    for (i = 0; i < rows; i++)
    {
      for (j = 0; j < cols; j++)
      {
        // numerator of (21)
        lij = (cols * rows * l) + (cols * i) + j;
        for (d = 0; d < dims; d++)
        {
          ld = dims * l + d;
          ijd = (dims * cols * i) + ( dims * j) + d;
          data->v[ld] += (data->y[ijd] - data->m[ld]) * (data->y[ijd] - data->m[ld]) * data->probs[lij];
        }
      }
    }
    if (data->N[l] != 0) for (d = 0; d < dims; d++)
    {
      ld = dims * l + d;
      data->v[ld] = data->v[ld] / data->N[l];
    }
  }

  for (l = 0; l < classes; l++)
   {
     if (data->v[l] < data->min_variance[l])
       data->v[l] = data->min_variance[l];
   }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMUtilities::MonitorMeansAndVariances(EMMPM_Data::Pointer dt)
{
  EMMPM_Data* data = dt.get();

  size_t l, d, ld;
  size_t classes = data->classes;
  size_t dims = data->dims;
  char msgbuff[256];
  memset(msgbuff, 0, 256);

  printf("Class\tDim\tMu\tVariance\n");

  for (l = 0; l < classes; l++)
  {
    for (d = 0; d < dims; d++)
    {
      ld = dims * l + d;
      printf("%d\t%d\t%3.3f\t%3.3f\n", (int)l, (int)d, data->m[l], data->v[l]);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMUtilities::RemoveZeroProbClasses(EMMPM_Data::Pointer dt)
{
  EMMPM_Data* data = dt.get();

  size_t kk, l, dd, ld, l1d, i, j, ij;
  size_t dims = data->dims;
  size_t rows = data->rows;
  size_t cols = data->columns;
  size_t classes = data->classes;

  for (kk = 0; kk < classes; kk++) {
    if (data->N[kk] == 0)
    {
      for (l = kk; l < classes - 1; l++)
      {
        /* Move other classes to fill the gap */
        data->N[l] = data->N[l + 1];
        for (dd = 0; dd < dims; dd++)
        {
          ld = dims * l + dd;
          l1d = (dims * (l+1)) + dd;
          data->m[ld] = data->m[l1d];
          data->v[ld] = data->v[l1d];
        }
      }
      for (i = 0; i < rows; i++)
      {
        for (j = 0; j < cols; j++)
        {
          ij = (cols*i) + j;
          if (data->xt[ij] > kk) data->xt[ij]--;
        }
      }
      classes = classes - 1; // push the eliminated class into the last class
    }
  }
}


