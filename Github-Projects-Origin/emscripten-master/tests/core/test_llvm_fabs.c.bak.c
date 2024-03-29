#include <assert.h>
#include <string.h>
#define INCLUDEMAIN
#include <math.h>
#include <stdio.h>
#include <float.h>
#include <emscripten/emscripten.h>

float __attribute__((noinline)) negative10()
{
  return (emscripten_random() < -1) ? 0.f : -10.5f;
}

float __attribute__((noinline)) positive42()
{
  return (emscripten_random() < -1) ? 0.f : 42.25f;
}

double __attribute__((noinline)) negative_dbl_max()
{
  return (emscripten_random() < -1) ? 0.f : -DBL_MAX;
}

int main()
{
  printf("%f\n", __builtin_fabsf(negative10()));
  printf("%f\n", __builtin_fabsf(positive42()));
  printf("%f\n", __builtin_fabsf((float)negative_dbl_max()));

  printf("%f\n", __builtin_fabs(negative10()));
  printf("%f\n", __builtin_fabs(positive42()));
  printf("%f\n", __builtin_fabs(negative_dbl_max()));
}
