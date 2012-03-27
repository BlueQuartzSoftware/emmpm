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

//-- MXA Includes
#include "MXA/MXA.h"
#include "MXA/Common/MXASetGetMacros.h"


//-- EM/MPM Lib Includes
#include "EMMPMLib/EMMPMLib.h"
#include "EMMPMLib/Common/EMMPM_Data.h"

/**
 * @brief
 * @author Michael A. Jackson (BlueQuartz Software)
 *
 */
class EMMPM_EXPORT InitializationFunction
{
  public:
    MXA_SHARED_POINTERS(InitializationFunction);
    MXA_STATIC_NEW_MACRO(InitializationFunction);
    MXA_TYPE_MACRO(InitializationFunction);
    virtual ~InitializationFunction();

    virtual void initialize(EMMPM_Data::Pointer data);

  protected:
    InitializationFunction();

  private:
    InitializationFunction(const InitializationFunction&); // Copy Constructor Not Implemented
    void operator=(const InitializationFunction&); // Operator '=' Not Implemented
};


/**
 * @brief The standard EM/MPM initialization functions
 * @author Michael A. Jackson (BlueQuartz Software)
 *
 */
class EMMPM_EXPORT BasicInitialization : public InitializationFunction
{

  public:
    MXA_SHARED_POINTERS(BasicInitialization);
    MXA_STATIC_NEW_MACRO(BasicInitialization);
    MXA_TYPE_MACRO(BasicInitialization);

    virtual ~BasicInitialization();

    void initialize(EMMPM_Data::Pointer data);

  protected:
    BasicInitialization();

  private:
    BasicInitialization(const BasicInitialization&); // Copy Constructor Not Implemented
    void operator=(const BasicInitialization&); // Operator '=' Not Implemented

};

/**
 * @brief Initialization using user defined areas of the input image
 * @author Michael A. Jackson (BlueQuartz Software)
 *
 */
class EMMPM_EXPORT UserDefinedAreasInitialization : public InitializationFunction
{

  public:
    MXA_SHARED_POINTERS(UserDefinedAreasInitialization);
    MXA_STATIC_NEW_MACRO(UserDefinedAreasInitialization);
    MXA_TYPE_MACRO(UserDefinedAreasInitialization);

    virtual ~UserDefinedAreasInitialization();

    void initialize(EMMPM_Data::Pointer data);

  protected:
    UserDefinedAreasInitialization();

  private:
    UserDefinedAreasInitialization(const UserDefinedAreasInitialization&); // Copy Constructor Not Implemented
    void operator=(const UserDefinedAreasInitialization&); // Operator '=' Not Implemented

};


/**
 * @brief Initializes the XT (Label Map) array
 * @author Michael A. Jackson (BlueQuartz Software)
 *
 */
class EMMPM_EXPORT XtArrayInitialization : public InitializationFunction
{

  public:
    MXA_SHARED_POINTERS(XtArrayInitialization);
    MXA_STATIC_NEW_MACRO(XtArrayInitialization);
    MXA_TYPE_MACRO(XtArrayInitialization);

    virtual ~XtArrayInitialization();

    void initialize(EMMPM_Data::Pointer data);

  protected:
    XtArrayInitialization();

  private:
    XtArrayInitialization(const XtArrayInitialization&); // Copy Constructor Not Implemented
    void operator=(const XtArrayInitialization&); // Operator '=' Not Implemented
};

/**
 * @brief Initializes the Gradient Variables
 * @author Michael A. Jackson (BlueQuartz Software)
 *
 */
class EMMPM_EXPORT GradientVariablesInitialization : public InitializationFunction
{

  public:
    MXA_SHARED_POINTERS(GradientVariablesInitialization);
    MXA_STATIC_NEW_MACRO(GradientVariablesInitialization);
    MXA_TYPE_MACRO(GradientVariablesInitialization);

    virtual ~GradientVariablesInitialization();

    void initialize(EMMPM_Data::Pointer data);

  protected:
    GradientVariablesInitialization();

  private:
    GradientVariablesInitialization(const GradientVariablesInitialization&); // Copy Constructor Not Implemented
    void operator=(const GradientVariablesInitialization&); // Operator '=' Not Implemented
};

/**
 * @brief Initialization using the curvature penalty functions
 * @author Michael A. Jackson (BlueQuartz Software)
 *
 */
class EMMPM_EXPORT CurvatureInitialization : public InitializationFunction
{

  public:
    MXA_SHARED_POINTERS(CurvatureInitialization);
    MXA_STATIC_NEW_MACRO(CurvatureInitialization);
    MXA_TYPE_MACRO(CurvatureInitialization);

    virtual ~CurvatureInitialization();

    void initialize(EMMPM_Data::Pointer data);

    /**
     * @brief Initialize the Curvature Penalty variables
     * @param data
     */
    void initCurvatureVariables(EMMPM_Data::Pointer data);

  protected:
    CurvatureInitialization();

  private:
    CurvatureInitialization(const CurvatureInitialization&); // Copy Constructor Not Implemented
    void operator=(const CurvatureInitialization&); // Operator '=' Not Implemented
};





#endif /* INITIALIZATIONFUNCTIONS_H_ */
