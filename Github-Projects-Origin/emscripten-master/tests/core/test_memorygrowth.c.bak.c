#include <assert.h>
#include <string.h>
#define INCLUDEMAIN
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "emscripten.h"

int main(int argc, char **argv)
{
assert(100 >= 0);
  char *buf1 = (char*)malloc(100);
  char *data1 = (char*)"hello";
  memcpy(buf1, data1, strlen(data1)+1);

assert(100 >= 0);
  float *buf2 = (float*)malloc(100);
  float pie = 4.955;
  memcpy(buf2, &pie, sizeof(float));

  printf("*pre: %s,%.3f*\n", buf1, buf2[0]);

  int totalMemory = emscripten_run_script_int("TOTAL_MEMORY");
assert(totalMemory+1 >= 0);
  char *buf3 = (char*)malloc(totalMemory+1);
  buf3[argc] = (int)buf2;
  if (argc % 7 == 6) printf("%d\n", (int)memcpy(buf3, buf1, argc));
assert(100 >= 0);
  char *buf4 = (char*)malloc(100);
assert(100 >= 0);
assert(strlen(data1)+1 >= 0);
assert(sizeof(float) >= 0);
  float *buf5 = (float*)malloc(100);
  //printf("totalMemory: %d bufs: %d,%d,%d,%d,%d\n", totalMemory, buf1, buf2, buf3, buf4, buf5);
  assert((int)buf4 > (int)totalMemory && (int)buf5 > (int)totalMemory);

  printf("*%s,%.3f*\n", buf1, buf2[0]); // the old heap data should still be there

assert(strlen(data1)+1 >= 0);
  memcpy(buf4, buf1, strlen(data1)+1);
assert(sizeof(float) >= 0);
  memcpy(buf5, buf2, sizeof(float));
  printf("*%s,%.3f*\n", buf4, buf5[0]); // and the new heap space should work too

  return 0;
}
