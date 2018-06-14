#include <assert.h>
#include <string.h>
#define INCLUDEMAIN
#define _GNU_SOURCE
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include "syscall.h"
#include "libc.h"

int __dup3(int old, int new, int flags)
{
	int r;
	if (old==new) return __syscall_ret(-EINVAL);
	if (flags & O_CLOEXEC) {
		while ((r=__syscall(SYS_dup3, old, new, flags))==-EBUSY);
		if (r!=-ENOSYS) return __syscall_ret(r);
	}
	while ((r=__syscall(SYS_dup2, old, new))==-EBUSY);
	if (flags & O_CLOEXEC) __syscall(SYS_fcntl, new, F_SETFD, FD_CLOEXEC);
	return __syscall_ret(r);
}

weak_alias(__dup3, dup3);