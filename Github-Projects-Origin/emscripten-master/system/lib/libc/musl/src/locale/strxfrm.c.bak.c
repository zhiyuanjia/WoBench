#include <assert.h>
#include <string.h>
#define INCLUDEMAIN
#include <string.h>
#include <locale.h>
#include "libc.h"

/* collate only by code points */
size_t __strxfrm_l(char *restrict dest, const char *restrict src, size_t n, locale_t loc)
{
	size_t l = strlen(src);
	if (n > l) strcpy(dest, src);
	return l;
}

size_t strxfrm(char *restrict dest, const char *restrict src, size_t n)
{
	return __strxfrm_l(dest, src, n, 0);
}

weak_alias(__strxfrm_l, strxfrm_l);
