///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EMTIFFIO_H_
#define EMTIFFIO_H_

#include "emmpm/common/EMMPMTypes.h"

#ifdef EMMPM_HAVE_STDINT_H
#include <stdint.h>
#endif

#include "emmpm/common/EMMPMTypes.h"
#include "emmpm/common/EMMPMVersion.h"
#include "MSVCDefines.h"



#ifdef __cplusplus
extern "C"
{
#endif

  /**
   * @brief
   * @param filename
   * @param width
   * @param height
   * @return
   */
  unsigned char** EM_ReadInputImage(char* filename, unsigned int* width, unsigned int* height);

  /**
   * @brief
   * @param filename
   * @param width
   * @param height
   * @param classes
   * @param xt
   * @return
   */
  int EM_WriteOutputImage(char* filename, unsigned int width, unsigned int height,
                          int classes, unsigned char** xt);


  /**
   * @brief
   * @param filename
   * @param width
   * @param height
   * @return
   */
  unsigned char* EM_ReadTiffAsGrayScale(char* filename, unsigned int* width, unsigned int* height);

  /**
   * @brief Writes the output from the EM/MPM into a Tiff File
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
