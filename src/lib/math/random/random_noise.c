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

void mt_init(unsigned long seed);
unsigned long mt_rand(void);

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

float rnd()
{
  float r = 0.f;
# ifdef RANDOM_STD
  r = (float)rand()/(float)RAND_MAX;
# endif
# ifdef RANDOM_MT
  r = (float)mt_rand()/(float)_SC_ULONG_MAX;
# endif
  return r;
}

// Define Gaussian PDF and CDF
float gaussianPDF(float x){return ((1.0)/(R2*RPI*SIG))*exp(-(x*x)/(2*SIG*SIG));}
float gaussianCDF(float x){return 0.5*(1+erf(x/ (R2*SIG)));}

// Newtons method to reconstruct noise value from
// random number thrown on [0,1).
float NEWTON(float(*PDF)(float), float(*CDF)(float), float V)
{
  float DELTA;
  float G = 0.0;
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
float genWhiteNoise(){return NEWTON(gaussianPDF, gaussianCDF, rnd());}

/* //////////////////////// */
// Brown Noise Generator
//
// For Brown noise audio, it is necessary to prevent the noise
// generator random walking too far from zero. So we need two
// bounding functions for nice clean brown noise.
//
float fixBrownNoiseU(float x){return NEWTON(gaussianPDF, gaussianCDF, x*rnd());}
float fixBrownNoiseL(float x){return NEWTON(gaussianPDF, gaussianCDF, (1- x*rnd()));}

float BN = 0;
float genBrownNoiseCorr(){
  if (BN> 600) {BN += fixBrownNoiseU(0.7);}
  else if (BN<-600) {BN += fixBrownNoiseL(0.3);}
  else {BN+=genWhiteNoise();}
  return BN;
}
float genBrownNoisePure(){
  BN+=genWhiteNoise();
  return BN;
}

/* ///////////////////////// */
// Violet Noise Generator
//
// Generate white noise and take discrete derivative
float VN = 0;
float genVioletNoise(){
  float TMP = VN;
  VN = genWhiteNoise();
  return TMP-VN;
}


////////////////////////
// Pink noise generator
//
// Complicated. Instead of generating a value, a seed vector of
// white noise is generated. The autocorrelation for 1/f noise
// is then invoked for all subsequent noise values.
float* autoCorr(int depth, float alpha){
  int n;
  float* A  = MEM_malloc_arrayN(depth, sizeof(float), "random_noise: autoCorr");
  for(n=0;n<depth;n++){
    if (n==0){A[n] = 1.0;}
    else{A[n] =((float)n - 1.0 - alpha/2)*A[n-1]/(float)n;}
  }
  return A;
}  

float* initPink(int depth, float alpha __UNUSED__){
  int n;
  float* PN = MEM_malloc_arrayN(depth - 1, sizeof(float), "random_noise: initPink");
  for(n=0;n<(depth-1);n++){PN[n] = genWhiteNoise();}
  return PN;
}

float genPinkNoise(float* P, float* A, int depth){
  int n; 
  // Create the V matrix
  float* V = MEM_malloc_arrayN(depth, sizeof(float), "random_noise: genPinkNoise");
  for (n=0;n<depth;n++){
    if (n==depth-1){V[n]=genWhiteNoise();}
    else{V[n] = -1*P[n];}
  }
  for (n=0;n<depth-1;n++){
    if (n==depth-2){P[n]=0.0;}
    else {P[n]=P[n+1];}
  }
  float dot=0.0;
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
float* initBlue(int depth, float alpha __UNUSED__){
  int n;
  float* bnoise = MEM_malloc_arrayN(depth - 1, sizeof(float), "random_noise: initBlue");
  for(n=0;n<(depth-1);n++){bnoise[n] = genVioletNoise();}
  return bnoise;
}

float genBlueNoise(float* B, float* A, int depth){
  int n; 
  // Create the V matrix
  float* V = MEM_malloc_arrayN(depth, sizeof(float), "random_noise: genBlueNoise");
  for (n=0;n<depth;n++){
    if (n==depth-1){V[n]=genVioletNoise();}
    else{V[n] = -1*B[n];}
  }
  for (n=0;n<depth-1;n++){
    if (n==depth-2){B[n]=0.0;}
    else {B[n]=B[n+1];}
  }
  float dot=0.0;
  for (n=0;n<depth;n++){dot+=V[n]*A[depth-1-n];}
  MEM_freeN(V);

  // Assign the dot product to the last value of PN
  B[depth-2] = dot;
  return B[depth-2];
}