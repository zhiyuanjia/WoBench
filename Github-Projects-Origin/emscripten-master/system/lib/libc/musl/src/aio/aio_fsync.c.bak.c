#include <assert.h>
#include <string.h>
#define INCLUDEMAIN
#include <aio.h>
#include <errno.h>

int aio_fsync(int op, struct aiocb *cb)
{
	/* FIXME: unsupported */
	errno = EINVAL;
	return -1;
}
