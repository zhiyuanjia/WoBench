#include <assert.h>
#include <string.h>
#define INCLUDEMAIN
/*
 * Copyright 2010 Tilera Corporation. All Rights Reserved.
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   as published by the Free Software Foundation, version 2.
 *
 *   This program is distributed in the hope that it will be useful, but
 *   WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, GOOD TITLE or
 *   NON INFRINGEMENT.  See the GNU General Public License for
 *   more details.
 */

#include <linux/sched.h>
#include <linux/sched/task_stack.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/kernel.h>
#include <linux/signal.h>
#include <linux/errno.h>
#include <linux/wait.h>
#include <linux/unistd.h>
#include <linux/stddef.h>
#include <linux/personality.h>
#include <linux/suspend.h>
#include <linux/ptrace.h>
#include <linux/elf.h>
#include <linux/compat.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <asm/processor.h>
#include <asm/ucontext.h>
#include <asm/sigframe.h>
#include <asm/syscalls.h>
#include <asm/vdso.h>
#include <arch/interrupts.h>

struct compat_ucontext {
	compat_ulong_t	  uc_flags;
	compat_uptr_t     uc_link;
	struct compat_sigaltstack	  uc_stack;
	struct sigcontext uc_mcontext;
	sigset_t	  uc_sigmask;	/* mask last for extensibility */
};

struct compat_rt_sigframe {
	unsigned char save_area[C_ABI_SAVE_AREA_SIZE]; /* caller save area */
	struct compat_siginfo info;
	struct compat_ucontext uc;
};

/* The assembly shim for this function arranges to ignore the return value. */
long compat_sys_rt_sigreturn(void)
{
	struct pt_regs *regs = current_pt_regs();
	struct compat_rt_sigframe __user *frame =
		(struct compat_rt_sigframe __user *) compat_ptr(regs->sp);
	sigset_t set;

	if (!access_ok(VERIFY_READ, frame, sizeof(*frame)))
		goto badframe;
	if (__copy_from_user(&set, &frame->uc.uc_sigmask, sizeof(set)))
		goto badframe;

	set_current_blocked(&set);

	if (restore_sigcontext(regs, &frame->uc.uc_mcontext))
		goto badframe;

	if (compat_restore_altstack(&frame->uc.uc_stack))
		goto badframe;

	return 0;

badframe:
	signal_fault("bad sigreturn frame", regs, frame, 0);
	return 0;
}

/*
 * Determine which stack to use..
 */
static inline void __user *compat_get_sigframe(struct k_sigaction *ka,
					       struct pt_regs *regs,
					       size_t frame_size)
{
	unsigned long sp;

	/* Default to using normal stack */
	sp = (unsigned long)compat_ptr(regs->sp);

	/*
	 * If we are on the alternate signal stack and would overflow
	 * it, don't.  Return an always-bogus address instead so we
	 * will die with SIGSEGV.
	 */
	if (on_sig_stack(sp) && !likely(on_sig_stack(sp - frame_size)))
		return (void __user __force *)-1UL;

	/* This is the X/Open sanctioned signal stack switching.  */
	if (ka->sa.sa_flags & SA_ONSTACK) {
		if (sas_ss_flags(sp) == 0)
			sp = current->sas_ss_sp + current->sas_ss_size;
	}

	sp -= frame_size;
	/*
	 * Align the stack pointer according to the TILE ABI,
	 * i.e. so that on function entry (sp & 15) == 0.
	 */
	sp &= -16UL;
	return (void __user *) sp;
}

int compat_setup_rt_frame(struct ksignal *ksig, sigset_t *set,
			  struct pt_regs *regs)
{
	unsigned long restorer;
	struct compat_rt_sigframe __user *frame;
	int err = 0, sig = ksig->sig;

	frame = compat_get_sigframe(&ksig->ka, regs, sizeof(*frame));

	if (!access_ok(VERIFY_WRITE, frame, sizeof(*frame)))
		goto err;

	/* Always write at least the signal number for the stack backtracer. */
	if (ksig->ka.sa.sa_flags & SA_SIGINFO) {
		/* At sigreturn time, restore the callee-save registers too. */
		err |= copy_siginfo_to_user32(&frame->info, &ksig->info);
		regs->flags |= PT_FLAGS_RESTORE_REGS;
	} else {
		err |= __put_user(ksig->info.si_signo, &frame->info.si_signo);
	}

	/* Create the ucontext.  */
	err |= __clear_user(&frame->save_area, sizeof(frame->save_area));
	err |= __put_user(0, &frame->uc.uc_flags);
	err |= __put_user(0, &frame->uc.uc_link);
	err |= __compat_save_altstack(&frame->uc.uc_stack, regs->sp);
	err |= setup_sigcontext(&frame->uc.uc_mcontext, regs);
	err |= __copy_to_user(&frame->uc.uc_sigmask, set, sizeof(*set));
	if (err)
		goto err;

	restorer = VDSO_SYM(&__vdso_rt_sigreturn);
	if (ksig->ka.sa.sa_flags & SA_RESTORER)
		restorer = ptr_to_compat_reg(ksig->ka.sa.sa_restorer);

	/*
	 * Set up registers for signal handler.
	 * Registers that we don't modify keep the value they had from
	 * user-space at the time we took the signal.
	 * We always pass siginfo and mcontext, regardless of SA_SIGINFO,
	 * since some things rely on this (e.g. glibc's debug/segfault.c).
	 */
	regs->pc = ptr_to_compat_reg(ksig->ka.sa.sa_handler);
	regs->ex1 = PL_ICS_EX1(USER_PL, 1); /* set crit sec in handler */
	regs->sp = ptr_to_compat_reg(frame);
	regs->lr = restorer;
	regs->regs[0] = (unsigned long) sig;
	regs->regs[1] = ptr_to_compat_reg(&frame->info);
	regs->regs[2] = ptr_to_compat_reg(&frame->uc);
	regs->flags |= PT_FLAGS_CALLER_SAVES;
	return 0;

err:
	trace_unhandled_signal("bad sigreturn frame", regs,
			      (unsigned long)frame, SIGSEGV);
	return -EFAULT;
}
