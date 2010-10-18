///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "emmpm/common/EMTiffIO.h"

#ifdef EMMPM_HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef EMMPM_HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef EMMPM_HAVE_STRING_H
#include <string.h>
#endif


//-- TIFF Headers
// We define _TIFF_DATA_TYPEDEFS_ here because EMMPMTypes.h has the exact type of typedefs
#define _TIFF_DATA_TYPEDEFS_ 1
#include <tiffio.h>

#include "emmpm/common/EMMPMVersion.h"
#include "emmpm/common/EMTime.h"
#include "emmpm/common/allocate.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EM_WriteOutputImage(char* filename, unsigned int width, unsigned int height,
                        int classes, unsigned char** xt)
{
  int err = 1;
  unsigned char* raster;
  int index;
  unsigned int i;
  unsigned int j;

  raster = (unsigned char*)_TIFFmalloc(width * height);
  index = 0;
  for (i = 0; i < height; i++)
  {
    for (j = 0; j < width; j++)
    {
      raster[index++] = (int)xt[i][j] * 255 / (classes - 1);
    }
  }

  err = EM_WriteGrayScaleTiff(raster, filename, width, height, filename, "Segmented with EM/MPM");
  if (err < 0)
  {
    printf("Error writing Tiff file %s\n", filename);
    return -1;
  }
  else
  {
    printf("Wrote output image %s\n", filename);
  }
  _TIFFfree(raster);

  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned char** EM_ReadInputImage(char* filename, unsigned int* width, unsigned int* height)
{
  unsigned char* raster;
  unsigned int i;
  unsigned int j;
  uint8_t* dst;
  unsigned char **y;

  raster = EM_ReadTiffAsGrayScale(filename, width, height);
  dst = raster;
  /* Copy input image to y[][] */
  int cols = *width;
  int rows = *height;

  y = (unsigned char **)get_img(cols, rows, sizeof(char));
  for (i = 0; i < cols; i++)
  {
    for (j = 0; j < rows; j++)
    {
      y[i][j] = *dst;
      ++dst;
    }
  }
  _TIFFfree( raster ); // Release the memory used to read the image
  return y;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned char* EM_ReadTiffAsGrayScale(char* filename, unsigned int* width, unsigned int* height)
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

  bitspersample = 0;
  samplesperpixel = 0;
  photometric = 0;
  orientation = 0;
  totalBytes = 0;

  in = TIFFOpen(filename, "r");
  if (in == NULL)
  {
    printf("Error Opening Tiff file with Absolute Path:\n %s\n", filename);
    exit(1);
  }


  err = TIFFGetField(in, TIFFTAG_IMAGEWIDTH, width);
  err = TIFFGetField(in, TIFFTAG_IMAGELENGTH, height);
  err = TIFFGetField(in, TIFFTAG_SAMPLESPERPIXEL, &samplesperpixel);
  err = TIFFGetField(in, TIFFTAG_BITSPERSAMPLE, &bitspersample);
  err = TIFFGetField(in, TIFFTAG_PHOTOMETRIC, &photometric);
  err = TIFFGetField(in, TIFFTAG_ORIENTATION, &orientation);

  totalBytes = width[0] * height[0] * 4;
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
  if (!TIFFReadRGBAImageOriented(in, (*width), (*height), (unsigned int*)(raster), ORIENTATION_TOPLEFT, 0))
  {
    _TIFFfree(raster);
    return NULL;
  }

  // Collapse the data down to a single channel, that will end up
  //  being the grayscale values
  pixel_count = (*width) * (*height);

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
int EM_WriteGrayScaleTiff(unsigned char* raster, char* filename, int width, int height, char* documentName, char* imageDescription)
{
  int err;
  TIFF *out;
  char* dateTime;
  char software[1024];
  tsize_t area;

  out = TIFFOpen(filename, "w");
  if (out == NULL)
  {
    printf("Could not open output file '%s' for writing.\n", filename);
    return -1;
  }

  err = 0;
  // set the basic values
  err = TIFFSetField(out, TIFFTAG_IMAGEWIDTH, (int)width);
  err = TIFFSetField(out, TIFFTAG_IMAGELENGTH, (int)height);
  err = TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);
  err = TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 1);
  err = TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, (int)height); // 1 strip


  dateTime = EM_TiffDateTime();
  err = TIFFSetField(out, TIFFTAG_DATETIME, dateTime);
  // String based tags
  if (NULL != documentName)
  {
    err = TIFFSetField(out, TIFFTAG_DOCUMENTNAME, documentName);
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
  snprintf(software, 1024, "%s using libTif", emmpm_PACKAGE_COMPLETE);

  err = TIFFSetField(out, TIFFTAG_SOFTWARE, software);

  err = TIFFSetField(out, TIFFTAG_HOSTCOMPUTER, EMMPM_SYSTEM);

  // Write the information to the file
  area = (tsize_t)(width * height);
  err = TIFFWriteEncodedStrip(out, 0, raster, area);
  if (err != area)
  {
    err = -1;
  }

  (void)TIFFClose(out);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EM_FreeImageBuffer(unsigned char* buffer)
{
  _TIFFfree(buffer);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned char* EM_AllocateImageBuffer(int width, int height, int samplesPerPixel)
{
  return (unsigned char*)_TIFFmalloc( width * height * samplesPerPixel);
}
