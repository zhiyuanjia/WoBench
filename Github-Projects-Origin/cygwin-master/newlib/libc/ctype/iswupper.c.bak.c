#include <assert.h>
#include <string.h>
#define INCLUDEMAIN
/*
FUNCTION
	<<iswupper>>, <<iswupper_l>>---uppercase wide character test

INDEX
	iswupper

INDEX
	iswupper_l

SYNOPSIS
	#include <wctype.h>
	int iswupper(wint_t <[c]>);

	#include <wctype.h>
	int iswupper_l(wint_t <[c]>, locale_t <[locale]>);

DESCRIPTION
<<iswupper>> is a function which classifies wide-character values that
have uppercase translations.

<<iswupper_l>> is like <<iswupper>> but performs the check based on the
locale specified by the locale object locale.  If <[locale]> is
LC_GLOBAL_LOCALE or not a valid locale object, the behaviour is undefined.

RETURNS
<<iswupper>>, <<iswupper_l>> return non-zero if <[c]> is a uppercase wide character.

PORTABILITY
<<iswupper>> is C99.
<<iswupper_l>> is POSIX-1.2008.

No supporting OS subroutines are required.
*/
#include <_ansi.h>
#include <wctype.h>

int
iswupper (wint_t c)
{
  return (towlower (c) != c);
}
