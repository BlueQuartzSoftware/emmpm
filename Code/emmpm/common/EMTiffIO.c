///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "EMTiffIO.h"
#include "EMTime.h"

#ifdef EMMPM_HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef EMMPM_HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef EMMPM_HAVE_STRING_H
#include <string.h>
#endif

//----------------------------------------------------------------------------
unsigned int TIFFReader_GetFormat(TIFFReader* reader)
{
//  unsigned int cc;
//  unsigned short red, green, blue;

  switch ( reader->Photometrics )
    {
    case PHOTOMETRIC_RGB:
    case PHOTOMETRIC_YCBCR:
      return RGB;
    case PHOTOMETRIC_MINISWHITE:
    case PHOTOMETRIC_MINISBLACK:
      return GRAYSCALE;
    case PHOTOMETRIC_PALETTE:
      printf("PHOTOMETRIC_PALETTE is NOT implemented. Reading behavior is undefined.\n");
#if 0
      for( cc=0; cc<256; cc++ )
      {

        reader->GetColor( cc, &red, &green, &blue );
        if ( red != green || red != blue )
        {
          reader->ImageFormat = vtkTIFFReader::PALETTE_RGB;
          return reader->ImageFormat;
        }
      }
      reader->ImageFormat = vtkTIFFReader::PALETTE_GRAYSCALE;
      return reader->ImageFormat;
#endif
    }
  return OTHER;
}


//-------------------------------------------------------------------------
int TIFFReader_Initialize(TIFFReader* reader)
{
  unsigned int page;

  if ( reader->tiff )
    {
    if ( !TIFFGetField(reader->tiff, TIFFTAG_IMAGEWIDTH, &reader->Width) ||
         !TIFFGetField(reader->tiff, TIFFTAG_IMAGELENGTH, &reader->Height) )
      {
      return 0;
      }

    // Get the resolution in each direction
    TIFFGetField(reader->tiff, TIFFTAG_XRESOLUTION, &reader->XResolution);
    TIFFGetField(reader->tiff, TIFFTAG_YRESOLUTION, &reader->YResolution);
    TIFFGetField(reader->tiff, TIFFTAG_RESOLUTIONUNIT, &reader->ResolutionUnit);

    // Check the number of pages. First by looking at the number of directories
    reader->NumberOfPages = TIFFNumberOfDirectories(reader->tiff);

    if(reader->NumberOfPages == 0)
      {
      if ( !TIFFGetField(reader->tiff,TIFFTAG_PAGENUMBER,&reader->CurrentPage,
          &reader->NumberOfPages))
        {
        // Some image programs encode the number of pages into the TIFF_DESCRIPTION tag
        // but we are going to ignore that for now.
        }
      }

    // If the number of pages is still zero we look if the image is tiled
    if(reader->NumberOfPages == 0 && TIFFIsTiled(reader->tiff))
      {
      reader->NumberOfTiles = TIFFNumberOfTiles(reader->tiff);

      if ( !TIFFGetField(reader->tiff,TIFFTAG_TILEWIDTH,&reader->TileWidth)
        || !TIFFGetField(reader->tiff,TIFFTAG_TILELENGTH,&reader->TileHeight)
        )
        {
        printf( "Cannot read tile width and tile length from file.\n");
        }
      else
        {
        reader->TileRows = reader->Height/reader->TileHeight;
        reader->TileColumns = reader->Width/reader->TileWidth;
        }
      }

    // Checking if the TIFF contains subfiles
    if(reader->NumberOfPages > 1)
      {
      reader->SubFiles = 0;

      for(page = 0; page < reader->NumberOfPages; page++)
        {
        long subfiletype = 6;
        if(TIFFGetField(reader->tiff, TIFFTAG_SUBFILETYPE, &subfiletype))
          {
          if(subfiletype == 0)
            {
            reader->SubFiles+=1;
            }
          }
        TIFFReadDirectory(reader->tiff);
        }

      // Set the directory to the first image
      TIFFSetDirectory(reader->tiff,0);
      }

      // TIFFTAG_ORIENTATION tag from the image
      // data and use it if available. If the tag is not found in the image data,
      // use ORIENTATION_BOTLEFT by default.
      int status =  TIFFGetField(reader->tiff, TIFFTAG_ORIENTATION,
                            &reader->Orientation);
      if( ! status )
        {
        reader->Orientation = ORIENTATION_BOTLEFT;
        }

    TIFFGetFieldDefaulted(reader->tiff, TIFFTAG_SAMPLESPERPIXEL,
                          &reader->SamplesPerPixel);
    TIFFGetFieldDefaulted(reader->tiff, TIFFTAG_COMPRESSION, &reader->Compression);
    TIFFGetFieldDefaulted(reader->tiff, TIFFTAG_BITSPERSAMPLE,
                          &reader->BitsPerSample);
    TIFFGetFieldDefaulted(reader->tiff, TIFFTAG_PLANARCONFIG, &reader->PlanarConfig);
    TIFFGetFieldDefaulted(reader->tiff, TIFFTAG_SAMPLEFORMAT, &reader->SampleFormat);

    // If TIFFGetField returns false, there's no Photometric Interpretation
    // set for this image, but that's a required field so we set a warning flag.
    // (Because the "Photometrics" field is an enum, we can't rely on setting
    // reader->Photometrics to some signal value.)
    if (TIFFGetField(reader->tiff, TIFFTAG_PHOTOMETRIC, &reader->Photometrics))
      {
      reader->HasValidPhotometricInterpretation = 1;
      }
    else
      {
      reader->HasValidPhotometricInterpretation = 0;
      }
    if ( !TIFFGetField(reader->tiff, TIFFTAG_TILEDEPTH, &reader->TileDepth) )
      {
      reader->TileDepth = 0;
      }
    }
  /* Allocate the in memory image buffer */
  reader->image = (uint8_t*)malloc(reader->SamplesPerPixel * reader->Width * reader->Height);

  reader->OutputExtent[0] = 0;
  reader->OutputExtent[1] = reader->Width - 1;
  reader->OutputExtent[2] = 0;
  reader->OutputExtent[3] = reader->Height - 1;
  reader->OutputExtent[4] = 0;
  reader->OutputExtent[5] = 0;

  reader->OutputIncrements[0] = 1;
  reader->OutputIncrements[1] = reader->Width;
  reader->OutputIncrements[2] = reader->Width * reader->Height;

  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TIFFReader_EvaluateImageAt( TIFFReader* reader, void* out, void* in )
{
  unsigned char *image = (unsigned char *)out;
  unsigned char *source = (unsigned char *)in;
  int increment;
  unsigned short red, green, blue, alpha;
  switch ( TIFFReader_GetFormat(reader) )
    {
    case GRAYSCALE:
      if ( reader->Photometrics == PHOTOMETRIC_MINISBLACK )
        {
        if(reader->SamplesPerPixel == 2 && reader->SampleFormat == SAMPLEFORMAT_UINT)
          {
          unsigned short *image_us = (unsigned short*)out;
          unsigned short *source_us = (unsigned short*)in;
          *image_us = *source_us;
          }
        else if(reader->SamplesPerPixel == 2 && reader->SampleFormat == SAMPLEFORMAT_INT)
          {
          short *image_us = (short*)out;
          short *source_us = (short*)in;
          *image_us = *source_us;
          }
        else if(reader->SamplesPerPixel == 1 && reader->SampleFormat == SAMPLEFORMAT_INT)
          {
          char *image_us = (char*)out;
          char *source_us = (char*)in;
          *image_us = *source_us;
          }
        else
          {
          *image = *source;
          }
        }
      else
        {
        *image = ~( *source );
        }
      increment = 1;
      break;
    case PALETTE_GRAYSCALE:
      printf("PALETTE_GRAYSCALE NOT supported. %s(%d)\n", __FILE__, __LINE__);
//      this->GetColor(*source, &red, &green, &blue);
//      *image = (unsigned char)(red); // red >> 8
//      increment = 1;
      break;
    case RGB:
      red   = *(source);
      green = *(source+1);
      blue  = *(source+2);
      *(image)   = red;
      *(image+1) = green;
      *(image+2) = blue;
      if ( reader->SamplesPerPixel == 4 )
        {
        alpha = *(source+3);
        *(image+3) = 255-alpha;
        }
      increment = reader->SamplesPerPixel;
      break;
    case PALETTE_RGB:
      printf("PALETTE_RGB NOT supported. %s(%d)\n", __FILE__, __LINE__);
//      if(this->GetDataScalarType() == VTK_UNSIGNED_SHORT)
//        {
//        unsigned short *image_us = (unsigned short*)out;
//        unsigned short *source_us = (unsigned short*)in;
//        this->GetColor(*source_us, &red, &green, &blue);
//        *(image_us)   = red << 8;
//        *(image_us+1) = green << 8;
//        *(image_us+2) = blue << 8;
//        }
//      else if(this->GetDataScalarType() == VTK_SHORT)
//        {
//        short *image_us = (short*)out;
//        short *source_us = (short*)in;
//        this->GetColor(*source_us, &red, &green, &blue);
//        *(image_us)   = red << 8;
//        *(image_us+1) = green << 8;
//        *(image_us+2) = blue << 8;
//        }
//      else if(this->GetDataScalarType() == VTK_CHAR)
//        {
//        this->GetColor(*source, &red, &green, &blue);
//        *(image)   = static_cast<char>(red >> 8);
//        *(image+1) = static_cast<char>(green >> 8);
//        *(image+2) = static_cast<char>(blue >> 8);
//        }
//      else
//        {
//        this->GetColor(*source, &red, &green, &blue);
//        *(image)   = static_cast<unsigned char>(red >> 8);
//        *(image+1) = static_cast<unsigned char>(green >> 8);
//        *(image+2) = static_cast<unsigned char>(blue >> 8);
//        }
      increment = 3;
      break;
    default:
      return 0;
    }

  return increment;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TIFFReader_ReadGenericImage(TIFFReader* reader)
{
  unsigned int isize;
  unsigned int cc;
  int row, inc, fileRow;
  int ix;
  tdata_t buf;

  isize = TIFFScanlineSize(reader->tiff);
  buf = _TIFFmalloc(isize);

  if (reader->PlanarConfig != PLANARCONFIG_CONTIG)
  {
    printf("This reader can only do PLANARCONFIG_CONTIG\n");
    _TIFFfree(buf);
    if (reader != NULL && reader->image != NULL)
    {
      free(reader->image);
      reader->image = NULL;
    }
    return;
    }

  switch ( TIFFReader_GetFormat(reader) )
    {
    default:
    case GRAYSCALE:
    case PALETTE_GRAYSCALE:
      inc = 1;
      break;
    case RGB:
      inc = reader->SamplesPerPixel;
      break;
    case PALETTE_RGB:
      inc = 3;
      break;
    }


  if(reader->SampleFormat == SAMPLEFORMAT_UINT
      && reader->SamplesPerPixel == 1
      && reader->BitsPerSample == 8)
    {
    unsigned char* image;
    if (reader->PlanarConfig == PLANARCONFIG_CONTIG)
      {
      for ( row = reader->OutputExtent[2]; row <= reader->OutputExtent[3]; row ++ )
        {
        // Flip from lower left origin to upper left if necessary.
        if (reader->Orientation == ORIENTATION_TOPLEFT)
          {
          fileRow = row;
          }
        else
          {
          fileRow = reader->Height - row - 1;
          }
        if (TIFFReadScanline(reader->tiff, buf, fileRow, 0) <= 0)
          {
          printf("Problem reading the row: %d \n", fileRow);
          break;
          }
        image = (unsigned char*)(reader->image) + (row - reader->OutputExtent[2]) * reader->OutputIncrements[1];

        // Copy the pixels into the output buffer
        cc = reader->OutputExtent[0] * reader->SamplesPerPixel;
        for (ix = reader->OutputExtent[0]; ix <= reader->OutputExtent[1]; ++ix)
          {
          inc = TIFFReader_EvaluateImageAt(reader, image, (unsigned char*)(buf) + cc );
          image += reader->OutputIncrements[0];
          cc += reader->SamplesPerPixel;
          }
        }
      }
      else
      {
        printf("TIFFReader_ReadGenericImage will ONLY read PLANARCONFIG_CONTIG type of tiff.\n");

      }
    }
    else
    {
      printf("TIFFReader_ReadGenericImage ONLY reads Unsigned Char type images currently. Add another\n");
      printf("option in file %s - Line %d\n", __FILE__, __LINE__);
    }
  _TIFFfree(buf);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TIFFReader* EM_ReadTiffImage(char* filename)
{
  TIFFReader* reader;
  int status;
  int pixel_count;
  uint8_t* src;
  uint8_t* dst;

  status = 0;
  src = NULL;
  dst = NULL;
  /* Allocate our TIFFReader structure */
  reader = malloc(sizeof(TIFFReader));
  if (NULL == reader)
  {
    printf("Could not allocate the TIFFReader structure.\n");
    return NULL;
  }
  /* Open the Tiff file for reading */
  reader->tiff = TIFFOpen(filename, "r");
  if (reader->tiff == NULL)
  {
    printf("Error Opening Tiff file with Path:\n %s\n", filename);
    free(reader);
    return NULL;
  }

  /* Initialize our structures to hold the image information */
  status = TIFFReader_Initialize(reader);
  if (status == 0)
  {
    (void) TIFFClose(reader->tiff); // Close the tiff structures
    free(reader);
  }

  /* Now read the data from the file */
  unsigned int format = TIFFReader_GetFormat(reader);

    switch(format)
  {
    case GRAYSCALE:
      TIFFReader_ReadGenericImage( reader );
      if (NULL == reader->image)
      {
        EM_FreeTIFFReader(reader);
        return NULL;
      }
      break;
    case PALETTE_RGB:
    case RGB:
      if (reader->image !=NULL)
      {
        free(reader->image);
        reader->image = (uint8_t*)malloc(reader->Width * reader->Height * 4);
      }
      if (!TIFFReadRGBAImageOriented(reader->tiff, reader->Width, reader->Height,
                                     (unsigned int*)(reader->image), ORIENTATION_TOPLEFT, 0)  )
        {
          EM_FreeTIFFReader(reader);
          return NULL;
        }

        // Collapse the data down to a single channel, that will end up
        //  being the grayscale values
        pixel_count = reader->Width * reader->Height;

        // The collapse is done IN PLACE
        src = reader->image;
        dst = reader->image;
        while (pixel_count > 0)
        {
          *(dst) = (unsigned char)((float)src[0] * 0.299f + (float)src[1] * 0.587f + (float)src[2] * 0.114f);
          dst++;
          src += 4; //skip ahead by 4 bytes because we read the raw array into an RGBA array.
          pixel_count--;
        }
      break;

    case PALETTE_GRAYSCALE:
      printf("PALETTE_GRAYSCALE Not Supported: %s(%d)\n", __FILE__ , __LINE__);
    break;
  default:
    break;
  }

  /* Close the libTiff TIFF pointer */
  (void) TIFFClose(reader->tiff); // Close the tiff structures
  reader->tiff = NULL;

  /* Return the pointer to the TIFFReader structure */
  return reader;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EM_FreeTIFFReader(TIFFReader* reader)
{
  if (reader != NULL && reader->image != NULL)
  {
    free(reader->image);
    reader->image = NULL;
  }
  free(reader);
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
