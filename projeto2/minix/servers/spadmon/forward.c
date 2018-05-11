/* Forward all calls that require writing to PM server. Some are forwarded a second time to kernel. */

#include "spadmon.h"
#include <lib.h>
#include <minix/callnr.h>
#include <minix/sysinfo.h>
#include <minix/syslib.h>

int do_setrunnable(void) {
	/* Forwards message to PM to get procnr before actually doing the work inside the kernel */
	message m;
	m.m_lc_pm_sig.pid = m_in.m_lc_pm_sig.pid;
	return (_syscall(PM_PROC_NR, PM_SETRUNNABLE, &m));
}

int do_setsleep(void) {
	/* Forward message to PM before forwarding to kernel */
	message m;

	memset(&m, 0, sizeof(m));
	m.m_lc_pm_sig.pid = m_in.m_lc_pm_sig.pid;
	return (_syscall(PM_PROC_NR, PM_SETSLEEP, &m));
}

int do_setstopped(void) {
	/* Forwards message to PM */
        message m;
        memset(&m, 0, sizeof(m));
        m.m_lc_pm_sig.pid = m_in.m_lc_pm_sig.pid;
        return (_syscall(PM_PROC_NR, PM_SETSTOPPED, &m));
}

int do_setzombie(void) {
	/* Forwards message to PM */
        message m;
        memset(&m, 0, sizeof(m));
        m.m_lc_pm_sig.pid = m_in.m_lc_pm_sig.pid;
        return (_syscall(PM_PROC_NR, PM_SETZOMBIE, &m));
}

int do_setexit(void) {
	/* Forwards message to PM's do_kill system call */
        message m;
        memset(&m, 0, sizeof(m));
        m.m_lc_pm_sig.pid = m_in.m_lc_pm_sig.pid;
	m.m_lc_pm_sig.nr = SIGKILL;
	return (_syscall(PM_PROC_NR, PM_KILL, &m));
}

