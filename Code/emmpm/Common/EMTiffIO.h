///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EMTIFFIO_H_
#define EMTIFFIO_H_

#include "EMMPM/Common/EMMPMTypes.h"
#include "EMMPM/Common/EMMPMVersion.h"
#include "MSVCDefines.h"


#ifdef EMMPM_HAVE_STDINT_H
#include <stdint.h>
#endif


//-- TIFF Headers
// We define _TIFF_DATA_TYPEDEFS_ here because EMMPMTypes.h has the exact type of typedefs
#define _TIFF_DATA_TYPEDEFS_ 1
#include <tiffio.h>



#ifdef __cplusplus
extern "C"
{
#endif
  enum { NOFORMAT, RGB, GRAYSCALE, PALETTE_RGB, PALETTE_GRAYSCALE, OTHER };

  /**
  * @struct TIFFReaderInternal
  */
  typedef struct
  {
    TIFF *tiff;
    char IsOpen;
    uint32_t Width;
    uint32_t Height;
    uint16_t NumberOfPages;
    uint16_t CurrentPage;
    uint16_t SamplesPerPixel;
    uint16_t Compression;
    uint16_t BitsPerSample;
    uint16_t Photometrics;
    char HasValidPhotometricInterpretation;
    uint16_t PlanarConfig;
    uint16_t Orientation;
    uint32_t TileDepth;
    uint32_t TileRows;
    uint32_t TileColumns;
    uint32_t TileWidth;
    uint32_t TileHeight;
    uint16_t NumberOfTiles;
    uint32_t SubFiles;
    uint32_t ResolutionUnit;
    float XResolution;
    float YResolution;
    int16_t SampleFormat;
    uint8_t* image;
    int OutputExtent[6];
    int OutputIncrements[3];
  } TIFFReader;


  TIFFReader* EM_ReadTiffImage(char* filename);

  void EM_FreeTIFFReader(TIFFReader* reader);



  /**
   *
   * @param filename
   * @param width
   * @param height
   * @return
   */
  unsigned char* EM_ReadTiffAsGrayScale(char* filename, unsigned int* width, unsigned int* height);

  /**
   * Writes the output from the EM/MPM into a Tiff File
   * @param raster EM/MPM Output
   * @param filename The filename to write to.
   * @param width Width of the image
   * @param height Heigh of the image
   * @return Zero Value or Negative on Error. Anything else is considered a success;
   */
  int EM_WriteGrayScaleTiff(unsigned char* raster, char* filename, int width, int height, char* documentName, char* imageDescription);

  /**
   *
   * @param raster A memory buffer that was allocated with _TIFFmalloc
   */
  void EM_FreeImageBuffer(unsigned char* buffer);

/**
 *
 * @param width
 * @param height
 * @param samplesPerPixel
 * @return
 */
  unsigned char* EM_AllocateImageBuffer(int width, int height, int samplesPerPixel);

#ifdef __cplusplus
}
#endif

#endif /* EMTIFFIO_H_ */
