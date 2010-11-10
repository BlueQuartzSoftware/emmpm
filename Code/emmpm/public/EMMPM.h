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

#include <stdlib.h>

#include "emmpm/common/EMMPMTypes.h"
#include "emmpm/public/EMMPM_Structures.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * @return
 */
EMMPM_Files* EMMPM_AllocateFilesStructure();

/**
 *
 * @return
 */
EMMPM_Inputs* EMMPM_AllocateInputsStructure();

/**
 *
 * @return
 */
EMMPM_WorkingVars* EMMPM_AllocatedWorkingVarsStructure();

/**
 *
 * @return
 */
EMMPM_Update* EMMPM_AllocateUpdateStructure();

/**
 *
 * @param ptr
 */
void EMMPM_FreeFilesStructure(EMMPM_Files* ptr);

/**
 *
 * @param ptr
 */
void EMMPM_FreeInputsStructure(EMMPM_Inputs* ptr);

/**
 *
 * @param ptr
 */
void EMMPM_FreedWorkingVarsStructure(EMMPM_WorkingVars* ptr);

/**
 *
 * @param update
 */
void EMMPM_FreeUpdateStructure(EMMPM_Update* update);



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
* @brief Sets a function to be used to provide progress information in the form
* of a text variable and a percent variable which should range between zero and
* one
* @param pt2Func The function to be called for progress feedback
*/
void EMMPM_SetProgressFunction(void (*pt2Func)(char*, float));


/**
* @brief This will set a Custom Initialization function for use by the EM/MPM
* algorithm. If this is NOT set then the basic random distribution
* is used instead.
* @param pt2Func
* @return
*/
void EMMPM_SetInitializationFunction(void (*pt2Func)(EMMPM_Inputs*, EMMPM_WorkingVars*));


/**
 * @brief This sets the UpdateStats callback function where an updated semented
 * image and the updaed mean and variance for all the classes can be calculated.
 * @param pt2Func
 */
void EMMPM_SetProgressStatsFunction(void (*pt2Func)(EMMPM_Update*));

/**
 * @brief This function is called at the bottom of each EM loop with an updated
 * segmented image. This can be useful to show the evolution of the segmentation
 * during the EM/MPM algorithm.
 * @param update
 */
void EMMPM_ProgressStats(EMMPM_Update* update);

/**
 * @brief This function will copy the input image into an internal data structure
 * for use during the algorithm
 * @param files
 * @param inputs
 * @param vars
 */
void EMMPM_ConvertInputImageToWorkingImage(EMMPM_Files* files,
                                           EMMPM_Inputs* inputs,
                                           EMMPM_WorkingVars* vars);
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
void EMMPM_ConvertXtToOutputImage(EMMPM_Files* files,
                                           EMMPM_Inputs* inputs,
                                           EMMPM_WorkingVars* vars);

/**
 * @brief Main entry point for running the EMMPM algorithm. The EMMPM_Inputs and
 * EMMPM_Files parameters mush be non NULL and properly initialized with input
 * values and input images. A new output Image will be allocated for you if one
 * is NOT provided. YOU are responsible for cleaning up the memory that is allocated
 * by that process.
 * @param files The input filenames and/or raw image array
 * @param inputs The main input parameters to the emmpm algorithm
 */
void EMMPM_Execute(EMMPM_Files* files, EMMPM_Inputs* inputs);


#ifdef __cplusplus
}
#endif


#endif /* EMMPM_H_ */
