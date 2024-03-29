#include <assert.h>
#include <string.h>
#define INCLUDEMAIN
/*
 * sim-errno.c -- return address of errno for current thread.
 *
 * Copyright (c) 1995, 2001 Cygnus Support
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 */


/* syscall handler branches here in case of error. */

extern int *__errno();

int
_cerror (e)
     int e;
{
  *__errno() = e;
  return -1;
}
