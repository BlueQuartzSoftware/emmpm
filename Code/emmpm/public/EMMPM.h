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

#ifndef EMMPM_H_
#define EMMPM_H_

#include "emmpm/common/CMPConfiguration.h"

#if CMP_HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include "emmpm/common/EMMPMTypes.h"
#include "emmpm/public/EMMPM_Structures.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * @return
 */
EMMPM_Data* EMMPM_AllocateDataStructure();

/**
 *
 * @return
 */
EMMPM_CallbackFunctions* EMMPM_AllocateCallbackFunctionStructure();

/**
 *
 * @param ptr
 */
void EMMPM_FreeDataStructure(EMMPM_Data* ptr);

/**
 *
 * @param ptr
 */
void EMMPM_FreeCallbackFunctionStructure(EMMPM_CallbackFunctions* ptr);



#if 0
/**
 * @brief Shows a message and a percentage progress using the current EMMPM_ProgressFunction
 * that has been set using the @see EMMPM_SetProgressFunction callback function.
 * This API should be used instead of directly calling the ProgressFunction itself
 * as this will guard against bad memory access as a check to make sure there is
 * a non-null function assigned to the EMMPM_ProgressFunction function.
 * @param message The message to be displayed
 * @param progress The amount of progress that has taken place between 0 and 100.
 */
void EMMPM_ShowProgress(char* message, float progress);

/**
 * @brief This function is called at the bottom of each EM loop with an updated
 * segmented image. This can be useful to show the evolution of the segmentation
 * during the EM/MPM algorithm.
 * @param update
 */
void EMMPM_ProgressStats(EMMPM_Data* update, EMMPM_CallbackFunctions* callbacks);
#endif


/**
 * @brief This function will copy the input image into an internal data structure
 * for use during the algorithm
 * @param files
 * @param inputs
 * @param vars
 */
void EMMPM_ConvertInputImageToWorkingImage(EMMPM_Data* update, EMMPM_CallbackFunctions* callbacks);
/**
 * @brief This function will copy the internal data structure that represents an
 * segmented image into a possibly newly allocated array. If the outputImage pointer
 * is NULL then a 'malloc' type function will allocate a new array. If you let this
 * happen for you then YOU need to call @see EMMPM_FreeTiffImageBuffer yourself
 * when the emmpm algorithm is complete. If you preallocate the array then your
 * own memory clean systems will work just fine.
 * @param files
 * @param inputs
 * @param vars
 */
void EMMPM_ConvertXtToOutputImage(EMMPM_Data* update, EMMPM_CallbackFunctions* callbacks);

/**
 * @brief Main entry point for running the EMMPM algorithm. The EMMPM_Inputs and
 * EMMPM_Files parameters mush be non NULL and properly initialized with input
 * values and input images. A new output Image will be allocated for you if one
 * is NOT provided. YOU are responsible for cleaning up the memory that is allocated
 * by that process.
 * @param files The input filenames and/or raw image array
 * @param inputs The main input parameters to the emmpm algorithm
 */
void EMMPM_Execute(EMMPM_Data* update, EMMPM_CallbackFunctions* callbacks);


#ifdef __cplusplus
}
#endif


#endif /* EMMPM_H_ */
