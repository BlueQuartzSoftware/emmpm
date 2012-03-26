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

#ifndef EM_H_
#define EM_H_

#include "EMMPMLib/EMMPMLib.h"
#include "EMMPMLib/public/EMMPM_Constants.h"
#include "EMMPMLib/public/EMMPM.h"

class EMMPM_EXPORT EMMPMUtilities
{
  public:
    virtual ~EMMPMUtilities()
    {
    }

    /**
     * @brief This function will copy the input image into an internal data structure
     * for use during the algorithm
     * @param data The Structure to use
     * @param callbacks The list of callback functions
     */
     static void ConvertInputImageToWorkingImage(EMMPM_Data::Pointer data);

    /**
     * @brief This function will copy the internal data structure that represents an
     * segmented image into a possibly newly allocated array. If the outputImage pointer
     * is NULL then a 'malloc' type function will allocate a new array. If you let this
     * happen for you then YOU need to call @see FreeTiffImageBuffer yourself
     * when the emmpm algorithm is complete. If you preallocate the array then your
     * own memory clean systems will work just fine.
     * @param data The Structure to use
     * @param callbacks The list of callback functions
     */
     static void ConvertXtToOutputImage(EMMPM_Data::Pointer data);


    /**
     * @brief Resets the mean and variance to Zero in preparation for another round
     * of EM loops
     * @param data EMMPM_Data inputs
     */
    static void ResetModelParameters(EMMPM_Data::Pointer data);

    /**
     * @brief Updates the mean and variance for each class
     * @param data EMMPM_Data inputs
     */
    static void UpdateMeansAndVariances(EMMPM_Data::Pointer data);

    /**
     * @brief Prints out the Mean and Variance values for each class. Primarily used
     * for debugging.
     * @param data EMMPM_Data inputs
     * @param callbacks a non NULL EMMPM_CallbackFunctions structure
     */
    static void MonitorMeansAndVariances(EMMPM_Data::Pointer data);

    /**
     * @brief Removes classes that have a Zero Probability. The removed classes are
     * simply collapsed into the next lower class value.
     * @param data EMMPM_Data inputs
     */
    static void RemoveZeroProbClasses(EMMPM_Data::Pointer data);

  protected:
    EMMPMUtilities()
    {
    }

  private:
    EMMPMUtilities(const EMMPMUtilities&); // Copy Constructor Not Implemented
    void operator=(const EMMPMUtilities&); // Operator '=' Not Implemented
};

#endif /* EM_H_ */
