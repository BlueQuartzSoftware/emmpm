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

#include "EMMPMInputParser.h"

#include <tclap/CmdLine.h>
#include <tclap/ValueArg.h>
#include "emmpm/common/EMMPMVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPMInputParser::EMMPMInputParser()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPMInputParser::~EMMPMInputParser()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EMMPMInputParser::parseGrayTable(const std::string &values, EMMPM_Inputs* inputs)
{
  std::string::size_type pos = values.find(",", 0);


  size_t index = 0;
 // unsigned int* cPtr = &(inputs->grayTable[index]);

  int n = sscanf(values.substr(0, pos).c_str(), "%d", &(inputs->grayTable[index]) );
  if (n != 1)
  {
    inputs->grayTable[index] = 0;
    return -1;
  }

  ++index;
  while(pos != std::string::npos && pos != values.size() - 1)
  {
    n = sscanf(values.substr(pos+1).c_str(), "%d", &(inputs->grayTable[index]) );
    pos = values.find(",", pos+1);
    ++index;
  }

  if (index != static_cast<size_t>(inputs->classes))
  {
    std::cout << "Mismatch between the number of classes declared [" << inputs->classes <<
        "] and the number of values declared for the gray level table. [" << index << "]. Both should match." << std::endl;
    return -1;
  }

  return 0;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EMMPMInputParser::parseInitCoords(const std::string &coords, EMMPM_Inputs* inputs)
{
  std::string::size_type pos = coords.find(":", 0);

  size_t index = 0;
  unsigned int* cPtr = inputs->initCoords[index];

  int n = sscanf(coords.substr(0, pos).c_str(), "%d,%d,%d,%d", &(cPtr[0]), &(cPtr[1]), &(cPtr[2]), &(cPtr[3]) );
  if (n != 4)
  {
    cPtr[0] = cPtr[1] = cPtr[2] = cPtr[3] = 0;
    return -1;
  }

  ++index;
  while(pos != std::string::npos && pos != coords.size() - 1)
  {
    cPtr = inputs->initCoords[index];
    n = sscanf(coords.substr(pos+1).c_str(), "%d,%d,%d,%d", &(cPtr[0]), &(cPtr[1]), &(cPtr[2]), &(cPtr[3]) );
    pos = coords.find(":", pos+1);
    ++index;
  }

  if (index != static_cast<size_t>(inputs->classes) )
  {
    std::cout << "Mismatch between the number of classes declared [" << inputs->classes <<
        "] and the number of sets of coordinated to use for initialization [" << index << "]" << std::endl;
    return -1;
  }
  return 0;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
char* EMMPMInputParser::setFileName(const std::string &fname)
{
  std::string::size_type size = fname.size() + 1;
  char* buf = NULL;
  if (size > 1)
  {
    buf = (char*)malloc(size);
    ::memset(buf, 0, size);
    strncpy(buf, fname.c_str(), size - 1);
  }
  return buf;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EMMPMInputParser::parseCLIArguments(int argc, char *argv[], EMMPM_Files* files, EMMPM_Inputs* inputs)
{
  if (NULL == files || NULL == inputs)
  {
    printf("Either the EMMPM_Files pointer or the EMMPM_Inputs pointer was null. Returning early.\n");
    return -1;
  }

  TCLAP::CmdLine cmd("EM/MPM", ' ', EMMPM::Version::Complete);

  TCLAP::ValueArg<std::string> in_inputFile("i", "inputfile", "MXA File to be used as input", false, "", "");
  cmd.add(in_inputFile);
  TCLAP::ValueArg<std::string> in_outputFile("o", "outputfile", "The New MXA Output File", false, "", "");
  cmd.add(in_outputFile);

  TCLAP::ValueArg<float> in_beta("b", "beta", "Beta Value", false, 1.0, "");
  cmd.add(in_beta);
  TCLAP::ValueArg<float> in_gamma("g", "gamma", "Gamma Value", false, 0.1, "");
  cmd.add(in_gamma);
  TCLAP::ValueArg<int> in_emIter("e", "emIter", "Number of EM Iterations", false, 10, "");
  cmd.add(in_emIter);
  TCLAP::ValueArg<int> in_mpmIter("m", "mpmIter", "Number of MPM Iterations", false, 5, "");
  cmd.add(in_mpmIter);
  TCLAP::ValueArg<int> in_numClasses("n", "numClasses", "The Number of classes or phases in the material", false, 2, "");
  cmd.add(in_numClasses);
  TCLAP::SwitchArg in_verbose("v", "verbose", "Verbose Output", false);
  cmd.add(in_verbose);

  TCLAP::SwitchArg simAnneal("s", "simanneal", "Use Simulated Annealing", false);
  cmd.add(simAnneal);

  TCLAP::ValueArg<int> initType("z", "inittype", "The initialization algorithm that should be performed", false, 0, "");
  cmd.add(initType);
  TCLAP::ValueArg<std::string> initcoords("", "coords", "The upper left (x,y) and lower right (x,y) pixel coordinate sets of each class to be used in the initialization algorithm where each set is separated by a colon ':'. An example is 487,192,507,212:0,332,60,392 for 2 class system.", false, "", "");
  cmd.add(initcoords);

  TCLAP::ValueArg<std::string> graytable( "", "graytable", "Set an lookup table for the gray values associated with each class. This can be used to combine classes together at file writing time.", false, "", "");
  cmd.add(graytable);

  try
  {
    int error = 0;
    cmd.parse(argc, argv);

    inputs->beta = in_beta.getValue();
    inputs->gamma = in_gamma.getValue();
    inputs->mpmIterations = in_mpmIter.getValue();
    inputs->emIterations = in_emIter.getValue();
    inputs->classes = in_numClasses.getValue();
    inputs->verbose = in_verbose.getValue();
    inputs->simulatedAnnealing = simAnneal.getValue();

    files->input_file_name = setFileName(in_inputFile.getValue() );
    if (files->input_file_name == NULL)
    {
      std::cout << "There was an error parsing the input file name. Did you use the '-i' argument to set an input file?" << std::endl;
      return -1;
    }

    files->output_file_name = setFileName(in_outputFile.getValue() );
    if (files->output_file_name == NULL)
    {
      std::cout << "There was an error parsing the output file name. Did you use the '-o' argument to set an input file?" << std::endl;
      return -1;
    }

    if (inputs->initType != 0) {
      error = parseInitCoords(initcoords.getValue(), inputs);
      if (error < 0)
      {
        std::cout << "There was an error parsing the command line arguments for the initialization coordinates." << std::endl;
        return -1;
      }
    }
    if (graytable.getValue().empty() == false)
    {
      error = parseGrayTable(graytable.getValue(), inputs);
      if (error < 0)
      {
        std::cout << "There was an error parsing the Gray Level Table." << std::endl;
        return -1;
      }
    }
    else // Create a default gray level table
    {
      int n = inputs->classes - 1;
      for (int value = 0; value < inputs->classes; ++value)
      {
       inputs->grayTable[value] = value * 255 / n;
      }
    }

  }
  catch (TCLAP::ArgException &e)
  {
    std::cerr << " error: " << e.error() << " for arg " << e.argId() << std::endl;
    std::cout << "** Unknown Arguments. Displaying help listing instead. **" << std::endl;
    return -1;
  }
  return 0;
}

