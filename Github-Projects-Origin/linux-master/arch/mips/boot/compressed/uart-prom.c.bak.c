#include <assert.h>
#include <string.h>
#define INCLUDEMAIN
// SPDX-License-Identifier: GPL-2.0

extern void prom_putchar(unsigned char ch);

void putc(char c)
{
	prom_putchar(c);
}