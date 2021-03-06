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

#ifndef INITIALIZATIONFUNCTIONS_H_
#define INITIALIZATIONFUNCTIONS_H_

#include "emmpm/common/DLLExport.h"
#include "emmpm/public/EMMPM_Structures.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the XT (Label Map) array
 * @param data
 */
EMMPM_EXPORT void EMMPM_InitializeXtArray(EMMPM_Data* data);


/**
 * @brief The standard EM/MPM initialization functions
 * @param data The EMMPM_Data Pointer
 */
EMMPM_EXPORT void EMMPM_BasicInitialization(EMMPM_Data* data);


/**
 * @brief Initialization using user defined areas of the input image
 * @param data The EMMPM_Data Pointer
 */
EMMPM_EXPORT void EMMPM_UserDefinedAreasInitialization(EMMPM_Data* data);


/**
 * @brief Initialization was already performed for each of the classes
 * @param data The EMMPM_Data Pointer
 */
EMMPM_EXPORT void EMMPM_ManualInitialization(EMMPM_Data* data);


/**
 * @brief Initialization using the curvature penalty functions
 * @param data The EMMPM_Data Pointer
 */
EMMPM_EXPORT void EMMPM_CurvatureInitialization(EMMPM_Data* data);


/**
 * @brief Initialize the Curvature Penalty variables
 * @param data
 */
EMMPM_EXPORT void EMMPM_InitCurvatureVariables(EMMPM_Data* data);

/**
 * @brief Initialize the Edge Gradient variables
 * @param data
 */
EMMPM_EXPORT void EMMPM_InitializeGradientVariables(EMMPM_Data* data);


#ifdef __cplusplus
}
#endif




#endif /* INITIALIZATIONFUNCTIONS_H_ */
