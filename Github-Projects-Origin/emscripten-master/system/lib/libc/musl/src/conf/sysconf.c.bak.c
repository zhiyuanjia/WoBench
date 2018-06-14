#include <assert.h>
#include <string.h>
#define INCLUDEMAIN
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <sys/resource.h>
#include <signal.h>
#include "syscall.h"
#include "libc.h"

#define VER (-2)
#define OFLOW (-3)
#define CPUCNT (-4)
#define RLIM(x) (-32768|(RLIMIT_ ## x))

long sysconf(int name)
{
	static const short values[] = {
		[_SC_ARG_MAX] = OFLOW,
		[_SC_CHILD_MAX] = RLIM(NPROC),
		[_SC_CLK_TCK] = 100,
		[_SC_NGROUPS_MAX] = 32,
		[_SC_OPEN_MAX] = RLIM(NOFILE),
		[_SC_STREAM_MAX] = -1,
		[_SC_TZNAME_MAX] = TZNAME_MAX,
		[_SC_JOB_CONTROL] = 1,
		[_SC_SAVED_IDS] = 1,
		[_SC_REALTIME_SIGNALS] = 1,
		[_SC_PRIORITY_SCHEDULING] = -1,
		[_SC_TIMERS] = VER,
		[_SC_ASYNCHRONOUS_IO] = VER,
		[_SC_PRIORITIZED_IO] = -1,
		[_SC_SYNCHRONIZED_IO] = -1,
		[_SC_FSYNC] = VER,
		[_SC_MAPPED_FILES] = VER,
		[_SC_MEMLOCK] = VER,
		[_SC_MEMLOCK_RANGE] = VER,
		[_SC_MEMORY_PROTECTION] = VER,
		[_SC_MESSAGE_PASSING] = VER,
		[_SC_SEMAPHORES] = VER,
		[_SC_SHARED_MEMORY_OBJECTS] = VER,
		[_SC_AIO_LISTIO_MAX] = -1,
		[_SC_AIO_MAX] = -1,
		[_SC_AIO_PRIO_DELTA_MAX] = 0, /* ?? */
		[_SC_DELAYTIMER_MAX] = _POSIX_DELAYTIMER_MAX,
		[_SC_MQ_OPEN_MAX] = -1,
		[_SC_MQ_PRIO_MAX] = OFLOW,
		[_SC_VERSION] = VER,
		[_SC_PAGE_SIZE] = OFLOW,
		[_SC_RTSIG_MAX] = _NSIG - 1 - 31 - 3,
		[_SC_SEM_NSEMS_MAX] = SEM_NSEMS_MAX,
		[_SC_SEM_VALUE_MAX] = OFLOW,
		[_SC_SIGQUEUE_MAX] = -1,
		[_SC_TIMER_MAX] = -1,
		[_SC_BC_BASE_MAX] = _POSIX2_BC_BASE_MAX,
		[_SC_BC_DIM_MAX] = _POSIX2_BC_DIM_MAX,
		[_SC_BC_SCALE_MAX] = _POSIX2_BC_SCALE_MAX,
		[_SC_BC_STRING_MAX] = _POSIX2_BC_STRING_MAX,
		[_SC_COLL_WEIGHTS_MAX] = COLL_WEIGHTS_MAX,
		[_SC_EQUIV_CLASS_MAX] = -1, /* ?? */
		[_SC_EXPR_NEST_MAX] = -1,
		[_SC_LINE_MAX] = -1,
		[_SC_RE_DUP_MAX] = RE_DUP_MAX,
		[_SC_CHARCLASS_NAME_MAX] = -1, /* ?? */
		[_SC_2_VERSION] = VER,
		[_SC_2_C_BIND] = VER,
		[_SC_2_C_DEV] = -1,
		[_SC_2_FORT_DEV] = -1,
		[_SC_2_FORT_RUN] = -1,
		[_SC_2_SW_DEV] = -1,
		[_SC_2_LOCALEDEF] = -1,
		[_SC_PII] = -1, /* ????????? */
		[_SC_PII_XTI] = -1,
		[_SC_PII_SOCKET] = -1,
		[_SC_PII_INTERNET] = -1,
		[_SC_PII_OSI] = -1,
		[_SC_POLL] = 1,
		[_SC_SELECT] = 1,
		[_SC_IOV_MAX] = IOV_MAX,
		[_SC_PII_INTERNET_STREAM] = -1,
		[_SC_PII_INTERNET_DGRAM] = -1,
		[_SC_PII_OSI_COTS] = -1,
		[_SC_PII_OSI_CLTS] = -1,
		[_SC_PII_OSI_M] = -1,
		[_SC_T_IOV_MAX] = -1,
		[_SC_THREADS] = VER,
		[_SC_THREAD_SAFE_FUNCTIONS] = VER,
		[_SC_GETGR_R_SIZE_MAX] = -1,
		[_SC_GETPW_R_SIZE_MAX] = -1,
		[_SC_LOGIN_NAME_MAX] = 256,
		[_SC_TTY_NAME_MAX] = TTY_NAME_MAX,
		[_SC_THREAD_DESTRUCTOR_ITERATIONS] = PTHREAD_DESTRUCTOR_ITERATIONS,
		[_SC_THREAD_KEYS_MAX] = PTHREAD_KEYS_MAX,
		[_SC_THREAD_STACK_MIN] = PTHREAD_STACK_MIN,
		[_SC_THREAD_THREADS_MAX] = -1,
		[_SC_THREAD_ATTR_STACKADDR] = VER,
		[_SC_THREAD_ATTR_STACKSIZE] = VER,
		[_SC_THREAD_PRIORITY_SCHEDULING] = VER,
		[_SC_THREAD_PRIO_INHERIT] = -1,
		[_SC_THREAD_PRIO_PROTECT] = -1,
		[_SC_THREAD_PROCESS_SHARED] = VER,
		[_SC_NPROCESSORS_CONF] = CPUCNT,
		[_SC_NPROCESSORS_ONLN] = CPUCNT,
		[_SC_PHYS_PAGES] = -1,
		[_SC_AVPHYS_PAGES] = -1,
		[_SC_ATEXIT_MAX] = -1,
		[_SC_PASS_MAX] = -1,
		[_SC_XOPEN_VERSION] = _XOPEN_VERSION,
		[_SC_XOPEN_XCU_VERSION] = _XOPEN_VERSION,
		[_SC_XOPEN_UNIX] = 1,
		[_SC_XOPEN_CRYPT] = -1,
		[_SC_XOPEN_ENH_I18N] = 1,
		[_SC_XOPEN_SHM] = 1,
		[_SC_2_CHAR_TERM] = -1,
		[_SC_2_C_VERSION] = -1,
		[_SC_2_UPE] = -1,
		[_SC_XOPEN_XPG2] = -1,
		[_SC_XOPEN_XPG3] = -1,
		[_SC_XOPEN_XPG4] = -1,
		[_SC_CHAR_BIT] = -1,
		[_SC_CHAR_MAX] = -1,
		[_SC_CHAR_MIN] = -1,
		[_SC_INT_MAX] = -1,
		[_SC_INT_MIN] = -1,
		[_SC_LONG_BIT] = -1,
		[_SC_WORD_BIT] = -1,
		[_SC_MB_LEN_MAX] = -1,
		[_SC_NZERO] = NZERO,
		[_SC_SSIZE_MAX] = -1,
		[_SC_SCHAR_MAX] = -1,
		[_SC_SCHAR_MIN] = -1,
		[_SC_SHRT_MAX] = -1,
		[_SC_SHRT_MIN] = -1,
		[_SC_UCHAR_MAX] = -1,
		[_SC_UINT_MAX] = -1,
		[_SC_ULONG_MAX] = -1,
		[_SC_USHRT_MAX] = -1,
		[_SC_NL_ARGMAX] = -1,
		[_SC_NL_LANGMAX] = -1,
		[_SC_NL_MSGMAX] = -1,
		[_SC_NL_NMAX] = -1,
		[_SC_NL_SETMAX] = -1,
		[_SC_NL_TEXTMAX] = -1,
		[_SC_XBS5_ILP32_OFF32] = -1,
		[_SC_XBS5_ILP32_OFFBIG] = 2*(sizeof(long)==4)-1,
		[_SC_XBS5_LP64_OFF64] = 2*(sizeof(long)==8)-1,
		[_SC_XBS5_LPBIG_OFFBIG] = -1,
		[_SC_XOPEN_LEGACY] = -1,
		[_SC_XOPEN_REALTIME] = -1,
		[_SC_XOPEN_REALTIME_THREADS] = -1,
		[_SC_ADVISORY_INFO] = VER,
		[_SC_BARRIERS] = VER,
		[_SC_BASE] = -1,
		[_SC_C_LANG_SUPPORT] = -1,
		[_SC_C_LANG_SUPPORT_R] = -1,
		[_SC_CLOCK_SELECTION] = VER,
		[_SC_CPUTIME] = VER,
		[_SC_THREAD_CPUTIME] = VER,
		[_SC_DEVICE_IO] = -1,
		[_SC_DEVICE_SPECIFIC] = -1,
		[_SC_DEVICE_SPECIFIC_R] = -1,
		[_SC_FD_MGMT] = -1,
		[_SC_FIFO] = -1,
		[_SC_PIPE] = -1,
		[_SC_FILE_ATTRIBUTES] = -1,
		[_SC_FILE_LOCKING] = -1,
		[_SC_FILE_SYSTEM] = -1,
		[_SC_MONOTONIC_CLOCK] = VER,
		[_SC_MULTI_PROCESS] = -1,
		[_SC_SINGLE_PROCESS] = -1,
		[_SC_NETWORKING] = -1,
		[_SC_READER_WRITER_LOCKS] = VER,
		[_SC_SPIN_LOCKS] = VER,
		[_SC_REGEXP] = 1,
		[_SC_REGEX_VERSION] = -1,
		[_SC_SHELL] = 1,
		[_SC_SIGNALS] = -1,
		[_SC_SPAWN] = VER,
		[_SC_SPORADIC_SERVER] = -1,
		[_SC_THREAD_SPORADIC_SERVER] = -1,
		[_SC_SYSTEM_DATABASE] = -1,
		[_SC_SYSTEM_DATABASE_R] = -1,
		[_SC_TIMEOUTS] = VER,
		[_SC_TYPED_MEMORY_OBJECTS] = -1,
		[_SC_USER_GROUPS] = -1,
		[_SC_USER_GROUPS_R] = -1,
		[_SC_2_PBS] = -1,
		[_SC_2_PBS_ACCOUNTING] = -1,
		[_SC_2_PBS_LOCATE] = -1,
		[_SC_2_PBS_MESSAGE] = -1,
		[_SC_2_PBS_TRACK] = -1,
		[_SC_SYMLOOP_MAX] = SYMLOOP_MAX,
		[_SC_STREAMS] = 0,
		[_SC_2_PBS_CHECKPOINT] = -1,
		[_SC_V6_ILP32_OFF32] = -1,
		[_SC_V6_ILP32_OFFBIG] = 2*(sizeof(long)==4)-1,
		[_SC_V6_LP64_OFF64] = 2*(sizeof(long)==8)-1,
		[_SC_V6_LPBIG_OFFBIG] = -1,
		[_SC_HOST_NAME_MAX] = HOST_NAME_MAX,
		[_SC_TRACE] = -1,
		[_SC_TRACE_EVENT_FILTER] = -1,
		[_SC_TRACE_INHERIT] = -1,
		[_SC_TRACE_LOG] = -1,

		[_SC_IPV6] = VER,
		[_SC_RAW_SOCKETS] = VER,
		[_SC_V7_ILP32_OFF32] = -1,
		[_SC_V7_ILP32_OFFBIG] = 2*(sizeof(long)==4)-1,
		[_SC_V7_LP64_OFF64] = 2*(sizeof(long)==8)-1,
		[_SC_V7_LPBIG_OFFBIG] = -1,
		[_SC_SS_REPL_MAX] = -1,
		[_SC_TRACE_EVENT_NAME_MAX] = -1,
		[_SC_TRACE_NAME_MAX] = -1,
		[_SC_TRACE_SYS_MAX] = -1,
		[_SC_TRACE_USER_EVENT_MAX] = -1,
		[_SC_XOPEN_STREAMS] = 0,
		[_SC_THREAD_ROBUST_PRIO_INHERIT] = -1,
		[_SC_THREAD_ROBUST_PRIO_PROTECT] = -1,
	};
	if (name > sizeof(values)/sizeof(values[0])) {
		errno = EINVAL;
		return -1;
	} else if (values[name] == VER) {
		return _POSIX_VERSION;
	} else if (values[name] == OFLOW) {
		if (name == _SC_ARG_MAX) return ARG_MAX;
		if (name == _SC_SEM_VALUE_MAX) return SEM_VALUE_MAX;
		if (name == _SC_MQ_PRIO_MAX) return MQ_PRIO_MAX;
		/* name == _SC_PAGE_SIZE */
		return PAGE_SIZE;
	} else if (values[name] == CPUCNT) {
		unsigned char set[128] = {1};
		int i, cnt;
		__syscall(SYS_sched_getaffinity, 0, sizeof set, set);
		for (i=cnt=0; i<sizeof set; i++)
			for (; set[i]; set[i]&=set[i]-1, cnt++);
		return cnt;
	} else if (values[name] < OFLOW) {
		long lim[2];
		__syscall(SYS_getrlimit, values[name]&16383, lim);
		return lim[0] < 0 ? LONG_MAX : lim[0];
	}
	return values[name];
}