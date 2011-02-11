/* random.h */

/* Created by Joel Dumke on 9/4/06
	This is just a header file for random.c. It was created for
	compatibility with VS .NET */

#ifndef _RANDOM_
#define	_RANDOM_





#ifdef __cplusplus
extern "C"
{
#endif


#ifdef USE_MERSENNE_TWISTER
#include "emmpm/mt/mt19937ar.h"
#else
/**
 * @brief Random Number Generator based off code from Park and Miller with
 * recommended adjustments.
 * @return
 */
double park_miller_rng();
double genrand_real2();
int genrand_real3();
void readseed();
void writeseed();
double normal();
double dexprand();
void init_genrand(unsigned long num);
#endif

#ifdef __cplusplus
}
#endif

#endif
