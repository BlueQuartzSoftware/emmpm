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
#ifndef EMTIFFIO_H_
#define EMTIFFIO_H_

#include "emmpm/public/EMMPM.h"


#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Reads an input image from an input file that is in the form of a Tiff image file
 * @param data The EMMPM_Data Structure. Must not be NULL.
 * @param callbacks The Callbacks structure to provide information to the user.
 * @return Negative on Error
 */
EMMPM_EXPORT int EMMPM_ReadInputImage(EMMPM_Data* data, EMMPM_CallbackFunctions* callbacks);

/**
 * @brief Writes the output from the EM/MPM algorithm to a tiff file
 * @param data The EMMPM_Data Structure. Must not be NULL.
 * @param callbacks The Callbacks structure to provide information to the user.
 * @return Negative on Error
 */
EMMPM_EXPORT int EMMPM_WriteOutputImage(EMMPM_Data* data, EMMPM_CallbackFunctions* callbacks);


/**
 * @brief Reads a tiff image value and converts it to a Grayscale image.
 * @param data The EMMPM_Data Structure. Must not be NULL.
 * @param callbacks The Callbacks structure to provide information to the user.
 * @return
 */
EMMPM_EXPORT unsigned char* EMMPM_ReadTiffAsGrayScale(EMMPM_Data* data, EMMPM_CallbackFunctions* callbacks);

/**
 * @brief Writes the output from the EM/MPM into a Tiff File
 * @param data The EMMPM_Data Structure. Must not be NULL.
 * @param callbacks The Callbacks structure to provide information to the user.
 * @param imageDescription Some descriptive text to store in the tiff image
 * @return Zero Value or Negative on Error. Anything else is considered a success;
 */
EMMPM_EXPORT int EMMPM_WriteGrayScaleTiff(EMMPM_Data* data,
                             EMMPM_CallbackFunctions* callbacks,
                          char* imageDescription);

/**
 * @brief Deallocates memory that is used to store image data. Typically allocated
 * with the _TIFFmalloc() function from libTif.
 * @param buffer A memory buffer that was allocated with _TIFFmalloc
 */
EMMPM_EXPORT void EMMPM_FreeTiffImageBuffer(unsigned char* buffer);

/**
 * @brief Allocates the proper Tiff Structures and Tiff raw image buffer
 * @param width Width of image
 * @param height Height of image
 * @param samplesPerPixel bits per pixel. 1 for grayscale, 3 for RGB
 * @return TIFF* image buffer
 */
EMMPM_EXPORT unsigned char* EMMPM_AllocateTiffImageBuffer(int width, int height, int samplesPerPixel);


/**
* @brief Writes the output from the EM/MPM Algorithm to a gray scale tiff image
* @param filename The filename to write the image to
* @param rows Number of rows in the image (height)
* @param columns Number of columns in the image (width)
* @param imageDescription Discription of what the image is
* @param image raw image buffer
* @return error code
*/
EMMPM_EXPORT int EMMPM_WriteGrayScaleImage(const char* filename, int rows, int columns,
                          const char* imageDescription, unsigned char* image);

#ifdef __cplusplus
}
#endif

#endif /* EMTIFFIO_H_ */
