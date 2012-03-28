/*
The Original EM/MPM algorithm was developed by Mary L. Comer and is distributed
under the BSD License.
Copyright (c) <2010>, <Mary L. Comer>
All rights reserved.

[1] Comer, Mary L., and Delp, Edward J.,  ÒThe EM/MPM Algorithm for Segmentation
of Textured Images: Analysis and Further Experimental Results,Ó IEEE Transactions
on Image Processing, Vol. 9, No. 10, October 2000, pp. 1731-1744.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list
of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

Neither the name of <Mary L. Comer> nor the names of its contributors may be
used to endorse or promote products derived from this software without specific
prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* acvmpm.c */

/* Modified by Joel Dumke on 1/30/09 */
#include "CurvatureMPM.h"


#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
//#include <boost/smart_ptr/detail/atomic_count.hpp>


#include "EMMPMLib/Common/EMMPM.h"
#include "EMMPMLib/Common/MSVCDefines.h"
#include "EMMPMLib/Common/EMMPM_Math.h"

#include "EMMPMLib/Common/EMTime.h"
#include "EMMPMLib/Common/EMMPMUtilities.h"



#if defined (EMMPMLib_USE_PARALLEL_ALGORITHMS)
#include <tbb/parallel_for.h>
#include <tbb/blocked_range2d.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

//static boost::detail::atomic_count counter(0);

class ParallelCalcLoop
{
  public:
    ParallelCalcLoop(EMMPM_Data* dPtr, real_t* ykPtr, real_t* rnd) :
    data(dPtr),
    yk(ykPtr),
    rnd(rnd)
    {}
    virtual ~ParallelCalcLoop(){}


    void calc(int rowStart, int rowEnd,
                  int colStart, int colEnd) const
    {
    //  ++counter;

      int l, prior;

      int32_t ij, lij, i1j1;
    //  int dims = data->dims;
      int rows = data->rows;
      int cols = data->columns;
      int classes = data->classes;

      real_t x, current;
      real_t post[EMMPM_MAX_CLASSES], sum, edge;

      size_t nsCols = data->columns - 1;
      size_t ewCols = data->columns;
      size_t swCols = data->columns - 1;
      size_t nwCols = data->columns - 1;

      unsigned char* xt = data->xt;
      real_t* probs = data->probs;
      real_t* ccost = data->ccost;
      real_t* ns = data->ns;
      real_t* ew = data->ew;
      real_t* sw = data->sw;
      real_t* nw = data->nw;
      real_t curvature_value = (real_t)0.0;


      for (int32_t i = rowStart; i < rowEnd; i++)
       {
         for (int32_t j = colStart; j < colEnd; j++)
         {
           ij = (cols * i) + j;
           sum = 0;
           for (l = 0; l < classes; l++)
           {
             /* edge penalties (in both x and y) */
             prior = 0;
             edge = 0;
             if (i - 1 >= 0)
             {
               if (j - 1 >= 0)
               {
                 i1j1 = (cols*(i-1))+j-1;
                 if (xt[i1j1] != l)
                 {
                   prior++;
                   i1j1 = (swCols*(i-1))+j-1;
                   if (data->useGradientPenalty) edge += sw[i1j1];
                 }
               }

               //Mark1
               i1j1 = (cols*(i-1))+j;
               if (xt[i1j1] != l)
               {
                 prior++;
                 i1j1 = (ewCols*(i-1))+j;
                 if (data->useGradientPenalty) edge += ew[i1j1];
               }
               //mark2
               if (j + 1 < cols)
               {
                 i1j1 = (cols*(i-1))+j+1;
                 if (xt[i1j1] != l)
                 {
                   prior++;
                   i1j1 = (nwCols*(i-1))+j;
                   if (data->useGradientPenalty) edge += nw[i1j1];
                 }
               }
             }

             //mark3
             if (i + 1 < rows)
             {
               if (j - 1 >= 0)
               {
                 i1j1 = (cols*(i+1))+j-1;
                 if (xt[i1j1] != l)
                 {
                   prior++;
                   i1j1 = (nwCols*(i))+j-1;
                   if (data->useGradientPenalty) edge += nw[i1j1];
                 }
               }
               //mark4
               i1j1 = (cols*(i+1))+j;
               if (xt[i1j1] != l)
               {
                 prior++;
                 i1j1 = (ewCols*(i))+j;
                 if (data->useGradientPenalty) edge += ew[i1j1];
               }
               //mark5
               if (j + 1 < cols)
               {
                 i1j1 = (cols*(i+1))+j+1;
                 if (xt[i1j1] != l)
                 {
                   prior++;
                   i1j1 = (swCols*(i))+j;
                   if (data->useGradientPenalty) edge += sw[i1j1];
                 }
               }
             }
             //mark6
             if (j - 1 >= 0)
             {
               i1j1 = (cols*(i))+j-1;
               if (xt[i1j1] != l)
               {
                 prior++;
                 i1j1 = (nsCols*(i))+j-1;
                 if (data->useGradientPenalty) edge += ns[i1j1];
               }
             }
             //mark7
             if (j + 1 < cols)
             {
               i1j1 = (cols*(i))+j+1;
               if (xt[i1j1] != l)
               {
                 prior++;
                 i1j1 = (nsCols*(i))+j;
                 if (data->useGradientPenalty) edge += ns[i1j1];
               }
             }
             lij = (cols * rows * l) + (cols * i) + j;
             curvature_value = 0.0;
             if (data->useCurvaturePenalty)
             {
               curvature_value = data->beta_c * ccost[lij];
             }
             float arg = yk[lij] - (data->workingBeta * (real_t)prior) - (edge) - (curvature_value) - data->w_gamma[l];
 //            if (arg < -708.3964)
 //            {
 //              hit++;
 //            }
             post[l] = expf(arg);
             sum += post[l];
           }
           x = rnd[ij];
           current = 0;
           for (l = 0; l < classes; l++)
           {
             lij = (cols * rows * l) + ij;
             //ij = (cols*i)+j;
             real_t arg = post[l]/sum;
             if ((x >= current) && (x <= (current + arg)))
             {
               xt[ij] = l;
               probs[lij] += 1.0;
             }
             current += arg;
           }
         }
       }

     }


#if defined (EMMPMLib_USE_PARALLEL_ALGORITHMS)
    void operator()(const tbb::blocked_range2d<int> &r) const
    {
      calc(r.rows().begin(), r.rows().end(), r.cols().begin(), r.cols().end());
    }
#endif


private:
    const EMMPM_Data* data;
    const real_t* yk;
    const real_t* rnd;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CurvatureMPM::CurvatureMPM() :
m_StatsDelegate(NULL)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CurvatureMPM::~CurvatureMPM()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CurvatureMPM::execute()
{
  EMMPM_Data* data = m_Data.get();

  real_t* yk;
  real_t sqrt2pi, con[EMMPM_MAX_CLASSES];
  real_t post[EMMPM_MAX_CLASSES];

  // int k, l;
 // unsigned int i, j, d;
  size_t ld, ijd, lij;
  unsigned int dims = data->dims;
  unsigned int rows = data->rows;
  unsigned int cols = data->columns;
  unsigned int classes = data->classes;

//  int rowEnd = rows/2;
  unsigned char* y = data->y;
  real_t* probs = data->probs;
  real_t* m = data->m;
  real_t* v = data->v;

  char msgbuff[256];
  float totalLoops;

  int currentLoopCount;

//  real_t curvature_value = (real_t)0.0;

  memset(post, 0, EMMPM_MAX_CLASSES * sizeof(real_t));
  memset(con, 0,  EMMPM_MAX_CLASSES * sizeof(real_t));

  totalLoops = (float)(data->emIterations * data->mpmIterations + data->mpmIterations);
  memset(msgbuff, 0, 256);
  data->progress++;

  yk = (real_t*)malloc(cols * rows * classes * sizeof(real_t));

  sqrt2pi = sqrt(2.0 * M_PI);

  for (uint32_t l = 0; l < classes; l++)
  {
    con[l] = 0;
    for (uint32_t d = 0; d < dims; d++)
    {
      ld = dims * l + d;
      con[l] += -log(sqrt2pi * sqrt(v[ld]));
    }
  }

  for (uint32_t i = 0; i < rows; i++)
  {
    for (uint32_t j = 0; j < cols; j++)
    {
      for (uint32_t l = 0; l < classes; l++)
      {
        lij = (cols * rows * l) + (cols * i) + j;
        probs[lij] = 0;
        yk[lij] = con[l];
        for (uint32_t d = 0; d < dims; d++)
        {
          ld = dims * l + d;
          ijd = (dims * cols * i) + (dims * j) + d;
          yk[lij] += ((y[ijd] - m[ld]) * (y[ijd] - m[ld]) / (-2.0 * v[ld]));
        }
      }
    }
  }

  const float rangeMin = 0;
  const float rangeMax = 1.0f;
  typedef boost::uniform_real<real_t> NumberDistribution;
  typedef boost::mt19937 RandomNumberGenerator;
  typedef boost::variate_generator<RandomNumberGenerator&,
                                   NumberDistribution> Generator;

  NumberDistribution distribution(rangeMin, rangeMax);
  RandomNumberGenerator generator;
  Generator numberGenerator(generator, distribution);
  generator.seed(EMMPM_getMilliSeconds()); // seed with the current time

  // Generate all the numbers up front
  size_t total = rows * cols;
  std::vector<real_t> rndNumbers(total);
  for(size_t i = 0; i < total; ++i)
  {
    rndNumbers[i] = numberGenerator();
  }

  unsigned long long int millis = EMMPM_getMilliSeconds();
  /* Perform the MPM loops */
  for (int32_t k = 0; k < data->mpmIterations; k++)
  {
    millis = EMMPM_getMilliSeconds();
    data->currentMPMLoop = k;
    if (data->cancel) { data->progress = 100.0; break; }
    data->inside_mpm_loop = 1;
  //  hit = 0;
#if defined (EMMPMLib_USE_PARALLEL_ALGORITHMS)
    tbb::task_scheduler_init init;
    int threads = init.default_num_threads();

    tbb::parallel_for(tbb::blocked_range2d<int>(0, rows, rows/threads, 0, cols, cols), ParallelCalcLoop(data, yk, &(rndNumbers.front())), tbb::simple_partitioner());
#else
    ParallelCalcLoop pcl(data, yk, &(rndNumbers.front()));
    pcl.calc(0, rows, 0, cols);
#endif

    //std::cout << "Counter: " << counter << std::endl;
    EMMPMUtilities::ConvertXtToOutputImage(getData());

    data->currentMPMLoop = k;
   // snprintf(msgbuff, 256, "MPM Loop %d", data->currentMPMLoop);
   // notify(msgbuff, 0, UpdateProgressMessage);

    currentLoopCount = data->mpmIterations * data->currentEMLoop + data->currentMPMLoop;
    data->progress = currentLoopCount / totalLoops * 100.0;

   // notify("", data->progress, UpdateProgressValue);
    if (m_StatsDelegate != NULL)
    {
      m_StatsDelegate->reportProgress(m_Data);
    }

  #if defined (EMMPMLib_USE_PARALLEL_ALGORITHMS)
    std::cout << "Parrallel MPM Loop Time to Complete:";
#else
    std::cout << "Serial MPM Loop Time To Complete: ";
#endif
    std::cout << (EMMPM_getMilliSeconds() - millis) << std::endl;

  }
  data->inside_mpm_loop = 0;

  if (!data->cancel)
  {
  /* Normalize probabilities */
    for (uint32_t i = 0; i < data->rows; i++)
    {
      for (uint32_t j = 0; j < data->columns; j++)
      {
        for (uint32_t l = 0; l < classes; l++)
        {
          lij = (cols * rows * l) + (cols * i) + j;
          data->probs[lij] = data->probs[lij] / (real_t)data->mpmIterations;
        }
      }
    }
  }

  /* Clean Up Memory */
  free(yk);

}
