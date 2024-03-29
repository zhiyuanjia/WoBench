#include <assert.h>
#include <string.h>
#define INCLUDEMAIN
#include <string.h>
#include <libgen.h>
#include "libc.h"

char *basename(char *s)
{
	size_t i;
	if (!s || !*s) return ".";
	i = strlen(s)-1;
	for (; i&&s[i]=='/'; i--) s[i] = 0;
	for (; i&&s[i-1]!='/'; i--);
	return s+i;
}

weak_alias(basename, __xpg_basename);
