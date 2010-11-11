/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "emmpm/common/io/EMTiffIO.h"

#ifdef CMP_HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef CMP_HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef CMP_HAVE_STRING_H
#include <string.h>
#endif


//-- TIFF Headers
// We define _TIFF_DATA_TYPEDEFS_ here because EMMPMTypes.h has the exact type of typedefs
#define _TIFF_DATA_TYPEDEFS_ 1
#include <tiffio.h>

#include "emmpm/common/EMMPMVersion.h"
#include "emmpm/common/utilities/EMTime.h"
#include "emmpm/common/utilities/allocate.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EMMPM_WriteOutputImage(EMMPM_Data* data, EMMPM_CallbackFunctions* callbacks)
{
  int err = 1;

  if (data->outputImage == NULL)
  {
    printf("Error: inputs->output_image was NULL. Can not write output image\n");
    return -1;
  }

  err = EMMPM_WriteGrayScaleTiff(data, callbacks, "Segmented with EM/MPM");
  if (err < 0)
  {
    printf("Error writing Tiff file %s\n", data->output_file_name);
    return -1;
  }

  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EMMPM_ReadInputImage(EMMPM_Data* data, EMMPM_CallbackFunctions* callbacks)
{
  int err = 0;

  data->inputImage = EMMPM_ReadTiffAsGrayScale(data, callbacks);
  data->channels = 1;

  if (NULL ==  data->inputImage)
  {
    if (callbacks->EMMPM_ProgressFunc != NULL) {
      callbacks->EMMPM_ProgressFunc("Error reading Tiff File", 0.0f);
    }
    return -1;
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned char* EMMPM_ReadTiffAsGrayScale(EMMPM_Data* data, EMMPM_CallbackFunctions* callbacks)
{
  TIFF* in;
  unsigned char* raster; /* retrieve RGBA image */
  int32_t pixel_count;
  unsigned char *src, *dst;
  size_t totalBytes;
  unsigned short samplesperpixel;
  unsigned short bitspersample;
  unsigned short photometric;
  unsigned short orientation;
  int err;
  int width, height;

  bitspersample = 0;
  samplesperpixel = 0;
  photometric = 0;
  orientation = 0;
  totalBytes = 0;

  in = TIFFOpen(data->input_file_name, "r");
  if (in == NULL)
  {
    printf("Error Opening Tiff file with Absolute Path:\n %s\n", data->input_file_name);
    exit(1);
  }


  err = TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &width);
  data->columns = width;
  err = TIFFGetField(in, TIFFTAG_IMAGELENGTH, &height);
  data->rows = height;
  err = TIFFGetField(in, TIFFTAG_SAMPLESPERPIXEL, &samplesperpixel);
  err = TIFFGetField(in, TIFFTAG_BITSPERSAMPLE, &bitspersample);
  err = TIFFGetField(in, TIFFTAG_PHOTOMETRIC, &photometric);
  err = TIFFGetField(in, TIFFTAG_ORIENTATION, &orientation);

  totalBytes = data->columns * data->rows * 4;
  raster = (unsigned char*)_TIFFmalloc( totalBytes );
  if (raster == NULL)
  {
    TIFFError(TIFFFileName(in), "No space for raster buffer");
    return NULL;
  }

  // TIFFReadRGBAImageOriented converts non 8bit images including:
  //  Grayscale, bilevel, CMYK, and YCbCR transparently into 32bit RGBA
  //  samples

  /* Read the image in one chunk into an RGBA array */
  if (!TIFFReadRGBAImageOriented(in, (data->columns), (data->rows), (unsigned int*)(raster), ORIENTATION_TOPLEFT, 0))
  {
    _TIFFfree(raster);
    return NULL;
  }

  // Collapse the data down to a single channel, that will end up
  //  being the grayscale values
  pixel_count = (data->rows) * (data->columns);

  // The collapse is done IN PLACE
  src = raster;
  dst = raster;
  while (pixel_count > 0)
  {
    *(dst) = (unsigned char)((float)src[0] * 0.299f + (float)src[1] * 0.587f + (float)src[2] * 0.114f);
    dst++;
    src += 4; //skip ahead by 4 bytes because we read the raw array into an RGBA array.
    pixel_count--;
  }
  (void) TIFFClose(in); // Close the tiff structures
  return raster;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int writeGrayScaleImage(const char* filename, int rows, int columns,
                        const char* imageDescription, unsigned char* image)
{

  int err;
   TIFF *out;
   char* dateTime;
   char software[1024];
   tsize_t area;

   if (NULL == image)
   {
     return -1;
   }
   out = TIFFOpen(filename, "w");
   if (out == NULL)
   {
     printf("Could not open output file '%s' for writing.\n", filename);
     return -1;
   }

   err = 0;
   // set the basic values
   err = TIFFSetField(out, TIFFTAG_IMAGEWIDTH, (int)columns);
   err = TIFFSetField(out, TIFFTAG_IMAGELENGTH, (int)rows);
   err = TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);
   err = TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 1);
   err = TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, (int)rows); // 1 strip


   dateTime = EM_TiffDateTime();
   err = TIFFSetField(out, TIFFTAG_DATETIME, dateTime);
   // String based tags
   if (NULL != filename)
   {
     err = TIFFSetField(out, TIFFTAG_DOCUMENTNAME, filename);
   }
   if (NULL != imageDescription)
   {
     err = TIFFSetField(out, TIFFTAG_IMAGEDESCRIPTION, imageDescription);
   }

   err = TIFFSetField(out, TIFFTAG_ORIENTATION, 1);
   err = TIFFSetField(out, TIFFTAG_PHOTOMETRIC, 1);

 #if USE_LZW_COMPRESSION
   err = TIFFSetField(image, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
   err = TIFFSetField(image, TIFFTAG_PREDICTOR, PREDICTOR_HORIZONTAL);
 #else
   err = TIFFSetField(out, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
 #endif

   // Insert Resolution Units here if possible


   memset(software, 0, 1024);
   snprintf(software, 1024, "%s using libTif", EMMPM_PACKAGE_COMPLETE);

   err = TIFFSetField(out, TIFFTAG_SOFTWARE, software);

   err = TIFFSetField(out, TIFFTAG_HOSTCOMPUTER, EMMPM_SYSTEM);

   // Write the information to the file
   area = (tsize_t)( columns *  rows);
   err = TIFFWriteEncodedStrip(out, 0, image, area);
   if (err != area)
   {
     err = -1;
   }
   else
   {
     err = 1;
   }

   (void)TIFFClose(out);
   return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EMMPM_WriteGrayScaleTiff(EMMPM_Data* data,
                             EMMPM_CallbackFunctions* callbacks,
                             char* imageDescription)
{
  return  writeGrayScaleImage(data->output_file_name,
                              data->rows, data->columns,
                              imageDescription, data->outputImage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM_FreeTiffImageBuffer(unsigned char* buffer)
{
  _TIFFfree(buffer);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned char* EMMPM_AllocateTiffImageBuffer(int width, int height, int samplesPerPixel)
{
  return (unsigned char*)_TIFFmalloc( width * height * samplesPerPixel);
}
