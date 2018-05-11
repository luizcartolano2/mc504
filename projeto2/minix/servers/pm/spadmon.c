
#include "pm.h"
#include <sys/wait.h>
#include <assert.h>
#include <minix/callnr.h>
#include <minix/com.h>
#include <minix/sched.h>
#include <minix/vm.h>
#include <sys/ptrace.h>
#include <sys/resource.h>
#include <signal.h>
#include "mproc.h"

#include <sys/exec.h>
#include <minix/sysinfo.h>

#include <machine/archtypes.h>
#include "/usr/src/minix/kernel/proc.h"

int get_procnr(int pid);

int do_setrunnable(void) {
	struct mproc *rmp;
	int procnr;
	message m;

        if((rmp = find_proc(m_in.m_lc_pm_sig.pid)) != NULL) {
                rmp->mp_flags &= ~TRACE_STOPPED;
                rmp->mp_flags &= ~(TRACE_ZOMBIE | ZOMBIE);
        }

	if ((procnr = get_procnr(m_in.m_lc_pm_sig.pid)) != -1) {
		m.m_lc_pm_sig.pid = procnr;
		return (_kernel_call(SYS_SETRUNNABLE, &m));
	}
	return 1;
}

int do_setsleep(void) {
	int pid, procnr, r, i;
	struct mproc *rmp;
	message m;

	if((rmp = find_proc(m_in.m_lc_pm_sig.pid)) != NULL) {
		rmp->mp_flags &= ~TRACE_STOPPED;
		rmp->mp_flags &= ~(TRACE_ZOMBIE | ZOMBIE);
	}

	if ((procnr = get_procnr(m_in.m_lc_pm_sig.pid)) != -1) {
		m.m_lc_pm_sig.pid = procnr;
		return (_kernel_call(SYS_SETSLEEP, &m));
	}
	return 1;
}

int do_setstopped(void) {
	struct mproc *rmp;
	
	rmp = find_proc(m_in.m_lc_pm_sig.pid);
	if (!(rmp->mp_flags & TRACE_STOPPED)) {
		rmp->mp_flags |= TRACE_STOPPED;
		return 0;
	}
	else {
		return 1;
	}
}

int do_setzombie(void) {
	struct mproc *rmp;
	rmp = find_proc(m_in.m_lc_pm_sig.pid);	
	if (!(rmp->mp_flags & (TRACE_ZOMBIE | ZOMBIE))) {
		rmp->mp_flags |= ZOMBIE;	/*If process wasn't a zombie, make it one.*/
		return 0;	
	}
	else {
		return 1; 			/* Process already was in zombie state, return error*/
	}
}

int do_setexit(void) {
	printf("Exit process here. PID is %d\n", m_in.m_lc_pm_sig.pid);
	
	return 6;
}

int get_procnr(int pid) {
        int procnr;
	struct mproc *rmp;
	int i;

	procnr = -1;
        for (i = 0; i < NR_PROCS; i++) {
                rmp = &mproc[i];
                if ((rmp->mp_flags & IN_USE) && rmp->mp_pid == pid) {
                        procnr = i;
			break;
                }
        }
	return procnr;
}
