/* C-noise: The open source noise generator  */ 
/* Copyright (C) 2015  Michael Winters  */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#include "mem/ensen_mem_guarded.h"

#include "ensen_private.h"
#include "ensen_math_random_noise.h"

// Define some constant
#define RPI 1.7724538509055159
#define R2 1.4142135623730951
#define PI 3.1415926535897932
#define COV 0.01
#define SIG 1.00

// Seed Random Number Generator
void init_rnd(){
  struct timeval tv;
  gettimeofday(&tv,NULL);
  srand(tv.tv_usec);
}

#define MT_N 624
#define MT_M 397

static unsigned long mt[MT_N];
static int mti = MT_N + 1;

/* 
  Initializes the Mersenne Twister algorithm with a seed value.
  To use: 
    srand(time(NULL));
    mt_init(rand());
 */
void 
mt_init(unsigned long seed) 
{
    mt[0] = seed;
    for (mti = 1; mti < MT_N; mti++) {
        mt[mti] = 1812433253UL * (mt[mti - 1] ^ (mt[mti - 1] >> 30)) + mti;
    }
}

/* 
  Generates a random number using the Mersenne Twister algorithm
 */
unsigned long 
mt_rand() 
{
    unsigned long y;
    static unsigned long mag01[2] = {0x0UL, 0x9908b0dfUL};
    if (mti >= MT_N) {
        int kk;
        if (mti == MT_N + 1) {
            mt_init(5489UL);
        }
        for (kk = 0; kk < MT_N - MT_M; kk++) {
            y = (mt[kk] & 0x80000000UL) | (mt[kk + 1] & 0x7fffffffUL);
            mt[kk] = mt[kk + MT_M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (; kk < MT_N - 1; kk++) {
            y = (mt[kk] & 0x80000000UL) | (mt[kk + 1] & 0x7fffffffUL);
            mt[kk] = mt[kk + (MT_M - MT_N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt[MT_N - 1] & 0x80000000UL) | (mt[0] & 0x7fffffffUL);
        mt[MT_N - 1] = mt[MT_M - 1] ^ (y >> 1) ^ mag01[y & 0x1UL];
        mti = 0;
    }
    y = mt[mti++];
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);
    return y;
}

# ifdef RANDOM_MT
data_t rnd()
{
  return (data_t)mt_rand()/(data_t)_SC_ULONG_MAX;
}
# else
data_t rnd()
{
  return (data_t)rand()/(data_t)RAND_MAX;
}
# endif

// Define Gaussian PDF and CDF
data_t gaussianPDF(data_t x){return ((1.0)/(R2*RPI*SIG))*exp(-(x*x)/(2*SIG*SIG));}
data_t gaussianCDF(data_t x){return 0.5*(1+erf(x/ (R2*SIG)));}

// Newtons method to reconstruct noise value from
// random number thrown on [0,1).
data_t NEWTON(data_t(*PDF)(data_t), data_t(*CDF)(data_t), data_t V)
{
  data_t DELTA;
  data_t G = 0.0;
  while(1){
    DELTA = (CDF(G) - V);
    if (fabs(DELTA) < COV){return G;}
    else{G -= (DELTA)/PDF(G);}	
  }
}

/* //////////////////////////////////////////////// */
/* //            NOISE GENERATORS                // */
/* //////////////////////////////////////////////// */


/* //////////////////////// */
// White Noise Generator
//
// The key to it all ! All function utilize genWhiteNoise a
// a basis for colored noise generation.
data_t genWhiteNoise(){return NEWTON(gaussianPDF, gaussianCDF, rnd());}

/* //////////////////////// */
// Brown Noise Generator
//
// For Brown noise audio, it is necessary to prevent the noise
// generator random walking too far from zero. So we need two
// bounding functions for nice clean brown noise.
//
data_t fixBrownNoiseU(data_t x){return NEWTON(gaussianPDF, gaussianCDF, x*rnd());}
data_t fixBrownNoiseL(data_t x){return NEWTON(gaussianPDF, gaussianCDF, (1- x*rnd()));}

data_t BN = 0;
data_t genBrownNoiseCorr(){
  if (BN> 600) {BN += fixBrownNoiseU(0.7);}
  else if (BN<-600) {BN += fixBrownNoiseL(0.3);}
  else {BN+=genWhiteNoise();}
  return BN;
}
data_t genBrownNoisePure(){
  BN+=genWhiteNoise();
  return BN;
}

/* ///////////////////////// */
// Violet Noise Generator
//
// Generate white noise and take discrete derivative
data_t VN = 0;
data_t genVioletNoise(){
  data_t TMP = VN;
  VN = genWhiteNoise();
  return TMP-VN;
}


////////////////////////
// Pink noise generator
//
// Complicated. Instead of generating a value, a seed vector of
// white noise is generated. The autocorrelation for 1/f noise
// is then invoked for all subsequent noise values.
data_t* autoCorr(int depth, data_t alpha){
  int n;
  data_t* A  = MEM_malloc_arrayN(depth, sizeof(data_t), "random_noise: autoCorr");
  for(n=0;n<depth;n++){
    if (n==0){A[n] = 1.0;}
    else{A[n] =((data_t)n - 1.0 - alpha/2)*A[n-1]/(data_t)n;}
  }
  return A;
}  

data_t* initPink(int depth, data_t alpha __UNUSED__){
  int n;
  data_t* PN = MEM_malloc_arrayN(depth - 1, sizeof(data_t), "random_noise: initPink");
  for(n=0;n<(depth-1);n++){PN[n] = genWhiteNoise();}
  return PN;
}

data_t genPinkNoise(data_t* P, data_t* A, int depth){
  int n; 
  // Create the V matrix
  data_t* V = MEM_malloc_arrayN(depth, sizeof(data_t), "random_noise: genPinkNoise");
  for (n=0;n<depth;n++){
    if (n==depth-1){V[n]=genWhiteNoise();}
    else{V[n] = -1*P[n];}
  }
  for (n=0;n<depth-1;n++){
    if (n==depth-2){P[n]=0.0;}
    else {P[n]=P[n+1];}
  }
  data_t dot=0.0;
  for (n=0;n<depth;n++){dot+=V[n]*A[depth-1-n];}
  MEM_freeN(V);
  
  // Assign the dot product to the last value of PN
  P[depth-2] = dot;
  return P[depth-2];
}

///////////////////////////////////////
// Azure Noise Generator
//
// Same thing as pink noise, only a violet noise vector is used as a
// seed vector. Note that the 'pinking' of violet noise produces blue
// noise (i.e. noise fractioning) 
data_t* initBlue(int depth, data_t alpha __UNUSED__){
  int n;
  data_t* bnoise = MEM_malloc_arrayN(depth - 1, sizeof(data_t), "random_noise: initBlue");
  for(n=0;n<(depth-1);n++){bnoise[n] = genVioletNoise();}
  return bnoise;
}

data_t genBlueNoise(data_t* B, data_t* A, int depth){
  int n; 
  // Create the V matrix
  data_t* V = MEM_malloc_arrayN(depth, sizeof(data_t), "random_noise: genBlueNoise");
  for (n=0;n<depth;n++){
    if (n==depth-1){V[n]=genVioletNoise();}
    else{V[n] = -1*B[n];}
  }
  for (n=0;n<depth-1;n++){
    if (n==depth-2){B[n]=0.0;}
    else {B[n]=B[n+1];}
  }
  data_t dot=0.0;
  for (n=0;n<depth;n++){dot+=V[n]*A[depth-1-n];}
  MEM_freeN(V);

  // Assign the dot product to the last value of PN
  B[depth-2] = dot;
  return B[depth-2];
}