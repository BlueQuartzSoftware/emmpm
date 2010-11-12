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
#ifndef _EMMPMINPUTPARSER_H_
#define _EMMPMINPUTPARSER_H_

#include <string>

#include "EMMPM.h"

/** @brief
 *
 */
class EMMPM_EXPORT EMMPMInputParser
{
  public:
    EMMPMInputParser();
    virtual ~EMMPMInputParser();

    /**
     * @brief
     * @param argc
     * @param argv
     * @param files
     * @param inputs
     * @return
     */
    int parseCLIArguments(int argc,char *argv[], EMMPM_Data* inputs);

    /**
     * @brief This will parse the pixel coordinates for each class that is to be initialized
     * @param coords
     * @param inputs
     * @return
     */
    int parseInitCoords(const std::string &coords, EMMPM_Data* inputs);

    /**
     * @brief
     * @param values
     * @param inputs
     * @return
     */
    int parseGrayTable(const std::string &values, EMMPM_Data* inputs);

    char* setFileName( const std::string &fname);

  private:
    EMMPMInputParser(const EMMPMInputParser&); // Copy Constructor Not Implemented
    void operator=(const EMMPMInputParser&); // Operator '=' Not Implemented
};

#endif /* _EMMPMINPUTPARSER_H_ */
