#include <assert.h>
#include <string.h>
#define INCLUDEMAIN
#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "trap.h"


int
close (int file)
{
  return TRAP0 (SYS_close, file, 0, 0);
}
