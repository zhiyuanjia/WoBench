#include <assert.h>
#include <string.h>
#define INCLUDEMAIN
/* 
 * This code was written by Rich Felker in 2010; no copyright is claimed.
 * This code is in the public domain. Attribution is appreciated but
 * unnecessary.
 */

#include <stdlib.h>
#include <wchar.h>

int wctomb(char *s, wchar_t wc)
{
	if (!s) return 0;
	return wcrtomb(s, wc, 0);
}
