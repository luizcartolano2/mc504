#include "kernel/system.h"
#include <string.h>
#include <minix/endpoint.h>
#include <minix/callnr.h>
#include "glo.h"
#include <minix/sysinfo.h>
#include <lib.h>

#if USE_SPADMON

int get_pos(int pid);

int do_setblocked(struct proc * caller, message *m_ptr) {
	struct proc *p;
	int pos;

	if ((pos = get_pos(m_ptr->m_lc_pm_sig.pid)) != -1) {
		p = proc_addr(pos);
		p->p_rts_flags &= RTS_RECEIVING;
		return (OK);
	}
	return (OK);
}

int do_setsleep(struct proc * caller, message *m_ptr) {
	struct proc *p;
	int pos;

	pos = m_ptr->m_lc_pm_sig.pid;
	p = proc_addr(pos);
	
	//If processe is running or stopped, we must change it.
	if (p->p_rts_flags == 0x0 || RTS_ISSET(p, RTS_P_STOP)) {
		p->p_rts_flags &= 0x0;
		p->p_rts_flags |= RTS_SLOT_FREE;
	}
	return OK;
}

int do_setrunnable(struct proc * caller, message *m_ptr) {
	struct proc *p;
	int pos;

        pos = m_ptr->m_lc_pm_sig.pid;
        p = proc_addr(pos);
        p->p_rts_flags &= 0x0;
	
	return OK;
}

int get_pos(int pid) {
        struct mproc *rmp;
        message m;
	int i;

        m.m_lsys_getsysinfo.what = SI_PROC_TAB;
        m.m_lsys_getsysinfo.where = (vir_bytes) kmproc;
        m.m_lsys_getsysinfo.size = sizeof(struct mproc) * NR_PROCS;

	printf("Going to call PM to copy proc table\n");

        _syscall(PM_PROC_NR, PM_GETSYSINFO, &m);

	printf("Returned from PM call to getsysinfo\n");

        for (i = 0; i < NR_PROCS; i++) {
                rmp = &kmproc[i];
                if (rmp->mp_pid == pid) {
			printf("Found the proc addr!\n");
                        return i;
                }
        }
	return -1;
}

#endif
