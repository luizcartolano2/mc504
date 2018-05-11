#include "spadmon.h"
#include <lib.h>
#include <minix/callnr.h>
#include <minix/sysinfo.h>
#include <minix/syslib.h>
#include "mproc.h"
#include <minix/sysinfo.h>
#include <machine/archtypes.h>
#include "/usr/src/minix/kernel/proc.h"
#include "sproc.h"

int do_getptable(void) {
	static struct proc proc_tab[NR_TASKS + NR_PROCS];
	static struct mproc mproc_tab[NR_PROCS];
        struct mproc *rmp;
	struct proc *kp;
	size_t len;
	vir_bytes src_addr, dst_addr;
	int i, r, kpid;

	if ((r = sys_getproctab(proc_tab)) != OK) {
		printf("SPADMON: unable to obtain kernel process table.\n");
		return 1;
	}
	if ((r = getsysinfo(PM_PROC_NR, SI_PROC_TAB, mproc_tab, sizeof(mproc_tab))) != OK) {
		printf("SPADMON: unable to obtain pm process table.\n");
		return 1;
	}


  for (i = 0; i < NR_PROCS; i++) {
    	rmp = &mproc_tab[i];

    	if ((rmp->mp_flags & IN_USE)) {
      	kpid = i + NR_TASKS;
      	kp = &proc_tab[kpid];
      	if (rmp->mp_flags & (TRACE_ZOMBIE | ZOMBIE))
        	pstate[i].state = 'Z';
      	else if ((rmp->mp_flags & TRACE_STOPPED) || RTS_ISSET(kp, RTS_P_STOP))
        	pstate[i].state = 'T';
      	else if (proc_is_runnable(kp))
        	pstate[i].state = 'R';
      	else
        	pstate[i].state = 'S';
    	}
    	else {
      		pstate[i].state = 'U';    /*Undefined state*/
    	}
    		pstate[i].blocked = P_BLOCKEDON(kp);
    		pstate[i].rts_flag = kp->p_rts_flags;
 		pstate[i].pid = rmp->mp_pid;
  	}

	src_addr = (vir_bytes) pstate;
	who_e = m_in.m_source;
  	len = sizeof(struct pstate) * NR_PROCS;
  	dst_addr = m_in.m_lsys_getsysinfo.where;
  	return(sys_datacopy(SELF, src_addr, who_e, dst_addr, len));
}
