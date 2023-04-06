#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "mem/ensen_mem_guarded.h"

#include "ensen_math_random.h"
#include "ensen_math_random_noise.h"
#include "ensen_private.h"

#define RPI 1.7724538509055159
#define R2 1.4142135623730951
#define PI 3.1415926535897932
#define COV 0.01
#define SIG 1.00

data_t
gaussianPDF(data_t x)
{
  return ((1.0) / (R2 * RPI * SIG)) * exp(-(x * x) / (2 * SIG * SIG));
}

data_t
gaussianCDF(data_t x)
{
  return 0.5 * (1 + erf(x / (R2 * SIG)));
}

data_t
NEWTON(data_t (*PDF)(data_t), data_t (*CDF)(data_t), data_t V)
{
  data_t DELTA;
  data_t G = 0.0;
  while (1)
  {
    DELTA = (CDF(G) - V);
    if (fabs(DELTA) < COV)
    {
      return G;
    }
    else
    {
      G -= (DELTA) / PDF(G);
    }
  }
}

data_t
genWhiteNoise(void)
{
  return NEWTON(gaussianPDF, gaussianCDF, rnd());
}

static data_t
fixBrownNoiseU(data_t x)
{
  return NEWTON(gaussianPDF, gaussianCDF, x * rnd());
}
static data_t
fixBrownNoiseL(data_t x)
{
  return NEWTON(gaussianPDF, gaussianCDF, (1 - x * rnd()));
}

data_t BN = 0;
data_t 
genBrownNoiseCorr(void)
{
  if (BN > 600)
  {
    BN += fixBrownNoiseU(0.7);
  }
  else if (BN < -600)
  {
    BN += fixBrownNoiseL(0.3);
  }
  else
  {
    BN += genWhiteNoise();
  }
  return BN;
}

data_t genBrownNoisePure(void)
{
  BN += genWhiteNoise();
  return BN;
}

data_t VN = 0;

data_t
genVioletNoise(void)
{
  data_t TMP = VN;
  VN = genWhiteNoise();
  return TMP - VN;
}

data_t *
autoCorr(int depth, data_t alpha)
{
  int n;
  data_t *A =
      MEM_malloc_arrayN(depth, sizeof(data_t), "random_noise: autoCorr");
  for (n = 0; n < depth; n++)
  {
    if (n == 0)
    {
      A[n] = 1.0;
    }
    else
    {
      A[n] = ((data_t)n - 1.0 - alpha / 2) * A[n - 1] / (data_t)n;
    }
  }
  return A;
}

data_t *
initPink(int depth, data_t alpha __UNUSED__)
{
  int n;
  data_t *PN =
      MEM_malloc_arrayN(depth - 1, sizeof(data_t), "random_noise: initPink");
  for (n = 0; n < (depth - 1); n++)
  {
    PN[n] = genWhiteNoise();
  }
  return PN;
}

data_t
genPinkNoise(data_t *P, data_t *A, int depth)
{
  int n;
  // Create the V matrix
  data_t *V =
      MEM_malloc_arrayN(depth, sizeof(data_t), "random_noise: genPinkNoise");
  for (n = 0; n < depth; n++)
  {
    if (n == depth - 1)
    {
      V[n] = genWhiteNoise();
    }
    else
    {
      V[n] = -1 * P[n];
    }
  }
  for (n = 0; n < depth - 1; n++)
  {
    if (n == depth - 2)
    {
      P[n] = 0.0;
    }
    else
    {
      P[n] = P[n + 1];
    }
  }
  data_t dot = 0.0;
  for (n = 0; n < depth; n++)
  {
    dot += V[n] * A[depth - 1 - n];
  }
  MEM_freeN(V);

  // Assign the dot product to the last value of PN
  P[depth - 2] = dot;
  return P[depth - 2];
}

data_t *
initBlue(int depth, data_t alpha __UNUSED__)
{
  int n;
  data_t *bnoise =
      MEM_malloc_arrayN(depth - 1, sizeof(data_t), "random_noise: initBlue");
  for (n = 0; n < (depth - 1); n++)
  {
    bnoise[n] = genVioletNoise();
  }
  return bnoise;
}

data_t
genBlueNoise(data_t *B, data_t *A, int depth)
{
  int n;
  // Create the V matrix
  data_t *V =
      MEM_malloc_arrayN(depth, sizeof(data_t), "random_noise: genBlueNoise");
  for (n = 0; n < depth; n++)
  {
    if (n == depth - 1)
    {
      V[n] = genVioletNoise();
    }
    else
    {
      V[n] = -1 * B[n];
    }
  }
  for (n = 0; n < depth - 1; n++)
  {
    if (n == depth - 2)
    {
      B[n] = 0.0;
    }
    else
    {
      B[n] = B[n + 1];
    }
  }
  data_t dot = 0.0;
  for (n = 0; n < depth; n++)
  {
    dot += V[n] * A[depth - 1 - n];
  }
  MEM_freeN(V);

  // Assign the dot product to the last value of PN
  B[depth - 2] = dot;
  return B[depth - 2];
}