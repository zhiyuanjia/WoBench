#include <assert.h>
#include <string.h>
#define INCLUDEMAIN
/* 
 * mutex7.c
 *
 * Test the default (type not set) mutex type.
 * Should be the same as PTHREAD_MUTEX_ERRORCHECK.
 * Thread locks then trylocks mutex (attempted recursive lock).
 * The thread should lock first time and EBUSY second time.
 *
 * Depends on API functions: 
 *	pthread_mutex_lock()
 *	pthread_mutex_trylock()
 *	pthread_mutex_unlock()
 */

#include "test.h"

static int lockCount = 0;

static pthread_mutex_t mutex;

void * locker(void * arg)
{
  assert(pthread_mutex_lock(&mutex) == 0);
  lockCount++;
  assert(pthread_mutex_trylock(&mutex) == EBUSY);
  lockCount++;
  assert(pthread_mutex_unlock(&mutex) == 0);
  assert(pthread_mutex_unlock(&mutex) == EPERM);

  return 0;
}
 
int
main()
{
  pthread_t t;

  assert(pthread_mutex_init(&mutex, NULL) == 0);

  assert(pthread_create(&t, NULL, locker, NULL) == 0);

  Sleep(1000);

  assert(lockCount == 2);

  exit(0);

  /* Never reached */
  return 0;
}