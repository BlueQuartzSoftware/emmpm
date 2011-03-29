/* random.h */

/* Created by Joel Dumke on 9/4/06
	This is just a header file for random.c. It was created for
	compatibility with VS .NET */

#ifndef _RANDOM_
#define	_RANDOM_


#include "emmpm/common/EMMPMConfiguration.h"


#ifdef __cplusplus
extern "C"
{
#endif


#ifdef USE_MERSENNE_TWISTER
#include "emmpm/mt/mt19937ar.h"
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
