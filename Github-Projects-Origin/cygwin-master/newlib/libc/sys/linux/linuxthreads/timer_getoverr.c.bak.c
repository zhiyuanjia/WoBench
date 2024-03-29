#include <assert.h>
#include <string.h>
#define INCLUDEMAIN
/* Copyright (C) 2000, 2001 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Kaz Kylheku <kaz@ashi.footprints.net>.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include <errno.h>
#include <pthread.h>
#include <time.h>

#include "posix-timer.h"


/* Get expiration overrun for timer TIMERID.  */
int
timer_getoverrun (timerid)
     timer_t timerid;
{
  struct timer_node *timer;
  int retval = -1;

  pthread_mutex_lock (&__timer_mutex);

  if (! timer_valid (timer = timer_id2ptr (timerid)))
    __set_errno (EINVAL);
  else
    retval = 0; /* TODO: overrun counting not supported */

  pthread_mutex_unlock (&__timer_mutex);

  return retval;
}
