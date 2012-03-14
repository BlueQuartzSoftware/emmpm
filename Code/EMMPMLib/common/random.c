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

/* random3(): modified on 10/23/89 from random2() to generate positive ints*/
/* Modified again on 9/4/06 by Joel Dumke for VS .NET */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>


#define MAXPRIME  2147483647       /*  MAXPRIME = (2^31)-1     */
/* #define PI        3.14159265358979323846 */

#include "EMMPMLib/common/EMMPM_Math.h"
#include "EMMPMLib/common/random.h"

/* PORTABILITY 1:  The functions in this file assume that a long is 32 bits
      and a short is 16 bits.  These conventions are machine dependent and
      must therefore be verified before using.                     */


/**  vars->emmpm_rnd_tmp: 31 bit seed in GF( (2^31)-1 )
* emmpm_seedBits[0]: high order 15 bits of vars->emmpm_rnd_tmp
* emmpm_seedBits[1]: low order 16 bits of vars->emmpm_rnd_tmp
* NOTE: high order 16 bits of emmpm_seedBits[0] and emmpm_seedBits[1] are 0
*/
//static unsigned int   emmpm_seedBits[2],vars->emmpm_rnd_tmp;


/** @brief Here is a portable C implementation of the ``minimal standard''
 * generator proposed by Park and Miller:
 */
#define PM_A 48271
#define PM_M 2147483647
#define PM_Q (PM_M / PM_A)
#define PM_R (PM_M % PM_A)

//static long int seed = 1;


/* Free the memory allocated for the random number state */
void freeRandStruct(RNGVars* vars)
{
  if (NULL != vars) { free(vars); }
}


double park_miller_rng(RNGVars* vars)
{
  assert(vars != NULL);
  long int hi = vars->seed / PM_Q;
  long int lo = vars->seed % PM_Q;
  long int test = PM_A * lo - PM_R * hi;
  if(test > 0)
    vars->seed = test;
  else  vars->seed = test + PM_M;
  return (double)vars->seed / PM_M;
}






/* Generates 20 random #'s starting from a seed of 1  */
/*
main()
{
 int    i;
 int  r,random3();

 srandom2(1);
 for(i=0;i<20;i++) {
    r=random3();
    printf("%d\n",r);
 }
}
*/
/*  TABLE:  The following are the first 20 random #'s which should be generated
         starting from a seed of 1:

16807
282475249
1622650073
984943658
1144108930
470211272
101027544
1457850878
1458777923
2007237709
823564440
1115438165
1784484492
74243042
114807987
1137522503
1441282327
16531729
823378840
143542612
                                          */

/*  Test for verifying the cycle length of the random # generator  */
/*   NOTE:  to speed up this test, comment out the return statement
                               in random2()                        */
/*
main()
{
 double random2();
 int i;

 srandom2(1);
 tmp=0;
 while (tmp!=1) {
   for(i=0;i<(256*256*256);i++) {
       random2();
       if (tmp == 1) break;
   }
   printf("*\n");
   if (tmp == 0) break;
   writeseed();
 }
 printf("\n%d\n",i);
 writeseed();
}
*/

/** @brief Set a new seed for random # generator  */
RNGVars* init_genrand(unsigned int num)
{
  RNGVars* vars = (RNGVars*)malloc(sizeof(RNGVars));
  vars->emmpm_rnd_tmp = num;
  vars->emmpm_seedBits[0] = vars->emmpm_rnd_tmp >> 16;
  vars->emmpm_seedBits[1] = vars->emmpm_rnd_tmp & 0xffff;
  return vars;
}

/* Uniform random number generator on (0,1] */
/*  Algorithm:  newseed = (16807 * oldseed) MOD [(2^31) - 1]  ;
                returned value = newseed / ( (2^31)-1 )  ;
      newseed is stored in tmp and sd[0] and sd[1] are updated;
      Since 16807 is a primitive element of GF[(2^31)-1], repeated calls
      to random2() should generate all positive integers <= MAXPRIME
      before repeating any one value.
    Tested: Feb. 16, 1988;  verified the length of cycle of integers
                             generated by repeated calls to random2()  */
double genrand_real2(RNGVars* vars)
{
  assert(vars != NULL);
  vars->emmpm_seedBits[1] *= 16807;
  vars->emmpm_seedBits[0] *= 16807;
  vars->emmpm_rnd_tmp = ((vars->emmpm_seedBits[0]) >> 15) + (((vars->emmpm_seedBits[0]) & 0x7fff) << 16);
  vars->emmpm_rnd_tmp += (vars->emmpm_seedBits[1]);
  if (vars->emmpm_rnd_tmp & 0x80000000)
  {
    vars->emmpm_rnd_tmp++;
    vars->emmpm_rnd_tmp &= 0x7fffffff;
  }
  vars->emmpm_seedBits[0] = vars->emmpm_rnd_tmp >> 16;
  vars->emmpm_seedBits[1] = vars->emmpm_rnd_tmp & 0xffff;
  return (((double)vars->emmpm_rnd_tmp) / MAXPRIME);
}


/* random3(): modified on 10/23/89 from random2() to generate positive ints*/
/* Uniform random number generator on (0,1] */
/*  Algorithm:  newseed = (16807 * oldseed) MOD [(2^31) - 1]  ;
                returned value = newseed / ( (2^31)-1 )  ;
      newseed is stored in tmp and sd[0] and sd[1] are updated;
      Since 16807 is a primitive element of GF[(2^31)-1], repeated calls
      to random2() should generate all positive integers <= MAXPRIME
      before repeating any one value.
    Tested: Feb. 16, 1988;  verified the length of cycle of integers
                             generated by repeated calls to random2()  */
int genrand_real3(RNGVars* vars)
{
  assert(vars != NULL);
  vars->emmpm_seedBits[1] *= 16807;
  vars->emmpm_seedBits[0] *= 16807;
  vars->emmpm_rnd_tmp = ((vars->emmpm_seedBits[0]) >> 15) + (((vars->emmpm_seedBits[0]) & 0x7fff) << 16);
  vars->emmpm_rnd_tmp += (vars->emmpm_seedBits[1]);
  if (vars->emmpm_rnd_tmp & 0x80000000)
  {
    vars->emmpm_rnd_tmp++;
    vars->emmpm_rnd_tmp &= 0x7fffffff;
  }
  vars->emmpm_seedBits[0] = vars->emmpm_rnd_tmp >> 16;
  vars->emmpm_seedBits[1] = vars->emmpm_rnd_tmp & 0xffff;
  return ((int)vars->emmpm_rnd_tmp);
}



/** @brief  Writes random # generator seed from file: /tmp/randomseedmlc */
void writeseed(RNGVars* vars)
{
  FILE* fp1;
  printf("+ Writing Random Seed '%d' to file /tmp/randomseedmlc", vars->emmpm_rnd_tmp);

  //char *calloc();
  fp1 = fopen("/tmp/randomseedmlc", "w");
  if ( NULL == fp1)
  {
    fprintf(stderr, "writeseed: can't open file /tmp/randomseedmlc\n");
    return;
  }
  else
  {
    fprintf(fp1, "%d", vars->emmpm_rnd_tmp);
    fclose(fp1);
  }
}

/** @brief  Reads random # generator seed from file: /tmp/randomseedmlc */
void readseed(RNGVars* vars)
{
  FILE* fp1;

  fp1 = fopen("/tmp/randomseedmlc", "r");
  if (NULL == fp1)
  {
    fprintf(stderr, "EMMPM.readseed():Could not read random seed file /tmp/randomseedmlc. Creating file\n");
    vars->emmpm_rnd_tmp = 143542612;
    writeseed(vars);
    init_genrand(vars->emmpm_rnd_tmp);
  }
  else
  {
    fscanf(fp1, "%d", &vars->emmpm_rnd_tmp);
    init_genrand(vars->emmpm_rnd_tmp);
    fclose(fp1);
  }
}


/** @brief  Generates normal random numbers: N(0,1)  */
double normal(RNGVars* vars)
{
  static int even = 1; /*   if  even = 0:  return b              */
  /*       even = 1:  compute 2 new values  */
  static double b; /*   temporary storage                    */
  double a, r, theta;

  if ((even = !even))
  {
    return (b);
  }
  else
  {
    theta = 2 * M_PI * genrand_real2(vars);
    r = sqrt(-2 * log(genrand_real2(vars)));
    a = r * cos(theta);
    b = r * sin(theta);
    return (a);
  }
}

/** @brief  Generates a double exponentially distributed random variable
      with mean 0 and variance 2.
*/
double dexprand(RNGVars* vars)
{
  double a, genrand_real2(vars);

  a = -log(genrand_real2(vars));
  if (genrand_real2(vars) > 0.5) a = (-a);
  return (a);
}




