/* random.c */

/* random3(): modified on 10/23/89 from random2() to generate positive ints*/
/* Modified again on 9/4/06 by Joel Dumke for VS .NET */

/* #include<pwd.h> */
#include <stdio.h>
//#include <stdlib.h>

#include <string.h>
#define MAXPRIME  2147483647       /*  MAXPRIME = (2^31)-1     */
#define PI        3.14159265358979323846

/* PORTABILITY 1:  The functions in this file assume that a long is 32 bits
      and a short is 16 bits.  These conventions are machine dependent and
      must therefore be verified before using.                     */

static unsigned int   sd[2],tmp;   /*  tmp: 31 bit seed in GF( (2^31)-1 )   */
                                    /*  sd[0]: high order 15 bits of tmp     */
                                    /*  sd[1]: low order 16 bits of tmp      */
                 /* NOTE: high order 16 bits of sd[0] and sd[1] are 0        */

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

double genrand_real2()
/* Uniform random number generator on (0,1] */
/*  Algorithm:  newseed = (16807 * oldseed) MOD [(2^31) - 1]  ;
                returned value = newseed / ( (2^31)-1 )  ;
      newseed is stored in tmp and sd[0] and sd[1] are updated;
      Since 16807 is a primitive element of GF[(2^31)-1], repeated calls
      to random2() should generate all positive integers <= MAXPRIME
      before repeating any one value.
    Tested: Feb. 16, 1988;  verified the length of cycle of integers 
                             generated by repeated calls to random2()  */
{
 *(sd+1) *= 16807;
 *sd *= 16807;
 tmp=((*sd)>>15)+(((*sd)&0x7fff)<<16);
 tmp += (*(sd+1));
 if ( tmp & 0x80000000 ) {
   tmp++;
   tmp &= 0x7fffffff;
 }
 *sd=tmp>>16;
 *(sd+1)=tmp & 0xffff;
 return(((double)tmp)/MAXPRIME);   
}

int genrand_real3()
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
{
 *(sd+1) *= 16807;
 *sd *= 16807;
 tmp=((*sd)>>15)+(((*sd)&0x7fff)<<16);
 tmp += (*(sd+1));
 if ( tmp & 0x80000000 ) {
   tmp++;
   tmp &= 0x7fffffff;
 }
 *sd=tmp>>16;
 *(sd+1)=tmp & 0xffff;
 return((int)tmp);   
}

init_genrand(unsigned int num)
/* Set a new seed for random # generator  */
{
 tmp=num;
 *sd=tmp>>16;
 *(sd+1)=tmp & 0xffff;
}

void readseed()
/*  Reads random # generator seed from file: /tmp/randomseedmlc */
{
 FILE	*fopen(),*fp1;
 char	*calloc();
 void	writeseed();

   if((fp1 = fopen("randomseedmlc","r"))==NULL ) {
     fprintf(stderr,"readseed: creating file randomseedmlc\n");
     tmp=143542612;
     writeseed();
     init_genrand(tmp);
   } else {
     fscanf(fp1,"%d",&tmp);
     init_genrand(tmp);
     fclose(fp1);
   }
}

void writeseed()
/*  Writes random # generator seed from file: /tmp/randomseedmlc */
{
 FILE  *fopen(),*fp1;
 char	*calloc();

   if((fp1 = fopen("randomseedmlc","w"))==NULL ) {
     fprintf(stderr,"writeseed: can't open file randomseedmlc\n");
     exit(1);
   } else {
     fprintf(fp1,"%d",tmp);
     fclose(fp1);
   }
}

double normal()
/*  Generates normal random numbers: N(0,1)  */
{
 static int   even = 1;  /*   if  even = 0:  return b              */
			 /*       even = 1:  compute 2 new values  */
 static double   b;      /*   temporary storage                    */
 double a,r,theta,random2();

 if((even=!even)) {
    return(b);
 } else {
    theta=2*PI*random2();
    r=sqrt(-2*log(random2()));
    a=r*cos(theta);
    b=r*sin(theta);
    return(a);
 }
}

double dexprand()
/*  Generates a double exponentially distributed random variable
      with mean 0 and variance 2.                                 */
{
 double  a,random2();

 a= -log(random2());
 if( random2()>0.5 )   a=(-a);
 return(a);
}
    



