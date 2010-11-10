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

#ifndef EMMPM_STRUCTURES_H_
#define EMMPM_STRUCTURES_H_


#include "emmpm/public/EMMPM_Constants.h"
#include "emmpm/common/EMMPMTypes.h"

/**
 * A structure to hold values that are useful to a callback function that would
 * like to display an image or indicate mu or sigma values
 */
typedef struct
{
    unsigned char* outputImage; /**<   */
    size_t        width; /**< The width of the image. Applicable for both input and output images */
    size_t        height; /**< The height of the image.  Applicable for both input and output images */
    size_t        channels; /**< The number of color channels in the images. This should always be 1 */
    int           classes; /**< The number of classes passed to the EM/MPM algorithm */
    double        m[MAX_CLASSES]; /**< The "mean" values for all the classes  */
    double        v[MAX_CLASSES]; /**< The "variance" values for all the classes  */
    int           currentEMLoop; /**< The current EM Loop  */
    int           currentMPMLoop; /**< The current MPM Loop  */
} EMMPM_Update;


/**
 * A Structure to hold the input and output file names and/or raw image data
 */
typedef struct
{

    char* input_file_name;/**< The input file name */
    unsigned char* inputImage; /**< The raw image data that is used as input to the algorithm */
    size_t        width; /**< The width of the image. Applicable for both input and output images */
    size_t        height; /**< The height of the image.  Applicable for both input and output images */
    size_t        channels; /**< The number of color channels in the images. This should always be 1 */
    char* output_file_name; /**< The name of the output file */
    unsigned char* outputImage; /**< The raw output image data which can be allocated by the library or the calling function. */
} EMMPM_Files;


/**
 * A Structure to hold all the inputs into the EM/MPM algorithm
 */
typedef struct
{
    int emIterations; /**<  */
    int mpmIterations; /**<  */
    double beta; /**<  */
    double gamma; /**<  */
    int classes; /**<  */
    unsigned int rows; /**<  */
    unsigned int columns; /**<  */
    unsigned int initType;  /**< The type of initialization algorithm to use  */
    unsigned int initCoords[MAX_CLASSES][4];  /**<  MAX_CLASSES rows x 4 Columns  */
    char simulatedAnnealing; /**<  */
    unsigned int grayTable[MAX_CLASSES];
    char verbose; /**<  */
} EMMPM_Inputs;

/**
 * Holds variables that are needed by the various functions through out
 * the EM/MPM algorithm. Packing them into a struct allows them to be easily
 * passed between functions.
 */
typedef struct
{
    unsigned char** y; /**<  */
    unsigned char** xt; /**<  */
    double gamma[MAX_CLASSES]; /**<  */
    double ga; /**<  */
    double x; /**<  */
    double m[MAX_CLASSES]; /**<  */
    double v[MAX_CLASSES]; /**<  */
    double N[MAX_CLASSES]; /**<  */
    double **probs[MAX_CLASSES]; /**<  */
    double workingBeta; /**<  */
    float progress; /**<  */
} EMMPM_WorkingVars;

#endif /* EMMPM_STRUCTURES_H_ */
