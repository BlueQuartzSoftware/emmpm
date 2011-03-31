/* random.h */

/* Created by Joel Dumke on 9/4/06
	This is just a header file for random.c. It was created for 
	compatibility with VS .NET */

#ifndef _RANDOM_
#define	_RANDOM_


double genrand_real2();
int genrand_real3();
void readseed();
void writeseed();
double normal();
double dexprand();
void init_genrand(unsigned int num);

#endif
