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




#include "emmpm/common/EMMPMTypes.h"
#include "emmpm/public/EMMPM_Structures.h"



#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Allocates an EMMPM_Data structure and initializes all the variables
 * @return Pointer to EMMPM_Data structure
 */
EMMPM_EXPORT EMMPM_Data* EMMPM_CreateDataStructure();


/**
 * @brief Allocates all the various arrays in the EMMPM_Data structure ONLY if
 * they are NOT null which means the programmer can allocate some of the arrays
 * before calling this function if need be.
 * @param data Non NULL EMMPM_Data Pointer
 * @return Negative on error
 */
EMMPM_EXPORT int EMMPM_AllocateDataStructureMemory(EMMPM_Data* data);


/**
 * @brief Allocated a EMMPM_CallbackFunctions structure but NOT the actual
 * callback functions
 * @return A EMMPM_CallbackFunctions Pointer
 */
EMMPM_EXPORT EMMPM_CallbackFunctions* EMMPM_AllocateCallbackFunctionStructure();

/**
 * @brief Frees all the memory in use by the EMMPM_Data structure including
 * arrays that may NOT have been allocated by the EMMPM Library. MAKE SURE you
 * copy data out of the structure if you need it past this call.
 * @param ptr Non NULL EMMPM_Data pointer which will be freed.
 */
EMMPM_EXPORT void EMMPM_FreeDataStructure(EMMPM_Data* ptr);

/**
 * @brief Deallocates the Callback function structure
 * @param ptr Non NULL EMMPM_CallbackFunctions pointer
 */
EMMPM_EXPORT void EMMPM_FreeCallbackFunctionStructure(EMMPM_CallbackFunctions* ptr);

/**
 * @brief This function will copy the input image into an internal data structure
 * for use during the algorithm
 * @param data The EMMPM_Structure to use
 * @param callbacks The list of callback functions
 */
EMMPM_EXPORT void EMMPM_ConvertInputImageToWorkingImage(EMMPM_Data* data, EMMPM_CallbackFunctions* callbacks);
/**
 * @brief This function will copy the internal data structure that represents an
 * segmented image into a possibly newly allocated array. If the outputImage pointer
 * is NULL then a 'malloc' type function will allocate a new array. If you let this
 * happen for you then YOU need to call @see EMMPM_FreeTiffImageBuffer yourself
 * when the emmpm algorithm is complete. If you preallocate the array then your
 * own memory clean systems will work just fine.
 * @param data The EMMPM_Structure to use
 * @param callbacks The list of callback functions
 */
EMMPM_EXPORT void EMMPM_ConvertXtToOutputImage(EMMPM_Data* data, EMMPM_CallbackFunctions* callbacks);

/**
 * @brief Main entry point for running the EMMPM algorithm. The EMMPM_Inputs and
 * EMMPM_Files parameters mush be non NULL and properly initialized with input
 * values and input images. A new output Image will be allocated for you if one
 * is NOT provided. YOU are responsible for cleaning up the memory that is allocated
 * by that process.
 * @param data The EMMPM_Structure to use
 * @param callbacks The list of callback functions
 */
EMMPM_EXPORT void EMMPM_Run(EMMPM_Data* data, EMMPM_CallbackFunctions* callbacks);


#ifdef __cplusplus
}
#endif


#endif /* EMMPM_H_ */
