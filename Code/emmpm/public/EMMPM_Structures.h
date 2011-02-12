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

enum EMMPM_Variation
{
  EMMPM_Basic = 0,
  EMMPM_UserInitArea,
  EMMPM_CurvaturePenalty
};


/**
 * Holds variables that are needed by the various functions through out
 * the EM/MPM algorithm. Packing them into a struct allows them to be easily
 * passed between functions.
 */
typedef struct
{

    // -----------------------------------------------------------------------------
    //  Inputs from Command line or GUI program
    // -----------------------------------------------------------------------------
    int emIterations; /**<  */
    int mpmIterations; /**<  */
    double in_beta; /**<  */
    int classes; /**<  */
    unsigned int rows; /**< The height of the image.  Applicable for both input and output images */
    unsigned int columns; /**< The width of the image. Applicable for both input and output images */
    unsigned int dims; /**< The number of vector elements in the image.*/
    unsigned int initType;  /**< The type of initialization algorithm to use  */
    unsigned int initCoords[EMMPM_MAX_CLASSES][4];  /**<  MAX_CLASSES rows x 4 Columns  */
    char simulatedAnnealing; /**<  */
    unsigned int grayTable[EMMPM_MAX_CLASSES];
    char verbose; /**<  */
    char inside_em_loop;
    char inside_mpm_loop;

    // -----------------------------------------------------------------------------
    //  Input/output File names and raw storage
    // -----------------------------------------------------------------------------
    char* input_file_name;/**< The input file name */
    unsigned char* inputImage; /**< The raw image data that is used as input to the algorithm */
    unsigned char inputImageChannels; /**< The number of color channels in the input image */
    char* output_file_name; /**< The name of the output file */
    unsigned char* outputImage; /**< The raw output image data which can be allocated by the library or the calling function. */

    enum EMMPM_Variation algorithm; /**< Which algorithm to use for the EM/MPM */

    // -----------------------------------------------------------------------------
    //  Working Vars section - Internal Variables to the algorithm
    // -----------------------------------------------------------------------------
    unsigned char* y; /**< height*width*dims array of bytes */
//    unsigned char* yvec; /**< */
    unsigned char* xt; /**< width*height array of bytes */

    double w_gamma[EMMPM_MAX_CLASSES]; /**<  */
    double* m; /**< classes * dims array (classes is fastest moving dimension)*/
    double* v; /**< classes * dims array */
    double N[EMMPM_MAX_CLASSES]; /**< classes * dims array */
    double* probs; /**< classes * rows * cols (slowest to fastest)*/
    double workingBeta; /**<  */

    // -----------------------------------------------------------------------------
    //  Curvature Penalty Function Related variables
    // -----------------------------------------------------------------------------
    double beta_e; /**<  */
    double beta_c; /**<  */
    double r_max; /**<  */
    double* ccost; /**< classes * rows * cols (slowest to fastest)*/
    double* ns; /**<  */
    double* ew; /**<  */
    double* sw; /**<  */
    double* nw; /**<  */


    // -----------------------------------------------------------------------------
    //  Variables that Functions may need to present progress information to the user
    // -----------------------------------------------------------------------------
    int           currentEMLoop; /**< The current EM Loop  */
    int           currentMPMLoop; /**< The current MPM Loop  */
    float         progress; /**< A Percentage to indicate how far along the algorthm is.*/

    // -----------------------------------------------------------------------------
    //  User defined Pointer that can point to anything
    // -----------------------------------------------------------------------------
    void*    userData; /**< User defined Pointer that can point to anything */
} EMMPM_Data;

/**
 *  A structure that holds the various callback functions that will be used
 *  throughout the code
 */
typedef struct
{
    void (*EMMPM_ProgressFunc)(char*, float); /**<  */
    void (*EMMPM_InitializationFunc)(EMMPM_Data*); /**<  */
    void (*EMMPM_ProgressStatsFunc)(EMMPM_Data*); /**<  */
} EMMPM_CallbackFunctions;



#endif /* EMMPM_STRUCTURES_H_ */
