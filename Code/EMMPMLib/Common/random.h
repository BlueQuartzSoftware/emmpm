/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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

/* Created by Joel Dumke on 9/4/06
	This is just a header file for random.c. It was created for
	compatibility with VS .NET */

#ifndef _RANDOM_
#define	_RANDOM_


#include "EMMPMLib/EMMPMLib.h"


#ifdef __cplusplus
extern "C"
{
#endif


#ifdef USE_MERSENNE_TWISTER
#include "EMMPMLib/mt/mt19937ar.h"
#else

/**  emmpm_rnd_tmp: 31 bit seed in GF( (2^31)-1 )
* emmpm_seedBits[0]: high order 15 bits of emmpm_rnd_tmp
* emmpm_seedBits[1]: low order 16 bits of emmpm_rnd_tmp
* NOTE: high order 16 bits of emmpm_seedBits[0] and emmpm_seedBits[1] are 0
*/
typedef struct
{
    unsigned int emmpm_seedBits[2];
    unsigned int emmpm_rnd_tmp;
    unsigned int seed;
} RNGVars;

/* Free the memory allocated for the random number state */
void freeRandStruct(RNGVars* vars);

RNGVars* init_genrand(unsigned int num);
/**
 * @brief Random Number Generator based off code from Park and Miller with
 * recommended adjustments.
 * @return
 */
double park_miller_rng(RNGVars* vars);
double genrand_real2(RNGVars* vars);
int genrand_real3(RNGVars* vars);

double normal(RNGVars* vars);
double dexprand(RNGVars* vars);

void readseed(RNGVars* vars);
void writeseed(RNGVars* vars);

#endif

#ifdef __cplusplus
}
#endif

#endif
