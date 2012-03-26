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

#ifndef CURVATUREEM_H_
#define CURVATUREEM_H_

#include "EMMPMLib/EMMPMLib.h"
#include "EMMPMLib/Common/Observable.h"
#include "EMMPMLib/Common/EMMPM_Data.h"
#include "EMMPMLib/Common/EMMPM_Constants.h"
#include "EMMPMLib/Common/EMMPM.h"
#include "EMMPMLib/Common/StatsDelegate.h"



class EMMPM_EXPORT CurvatureEM : public Observable
{
  public:
    MXA_SHARED_POINTERS(CurvatureEM)
    MXA_STATIC_NEW_MACRO(CurvatureEM)
    MXA_TYPE_MACRO(CurvatureEM)

    virtual ~CurvatureEM();

    MXA_INSTANCE_PROPERTY(EMMPM_Data::Pointer, Data);
    MXA_INSTANCE_PROPERTY(StatsDelegate::Pointer, StatsDelegate);

    /**
     * @fn void EMMPM_CurvatureEMLoops(EMMPM_Data* data, EMMPM_CallbackFunctions* callbacks)
     * @brief Runs the EM portion of the EM/MPM Algorithm
     * @param data a non NULL EMMPM_Data pointer
     * @param callbacks a non NULL EMMPM_CallbackFunctions
     */
    void execute();

  protected:
    CurvatureEM();

  private:
    CurvatureEM(const CurvatureEM&); // Copy Constructor Not Implemented
    void operator=(const CurvatureEM&); // Operator '=' Not Implemented

};

#endif /* CURVATUREEM_H_ */
