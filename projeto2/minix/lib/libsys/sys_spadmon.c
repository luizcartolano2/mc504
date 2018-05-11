#include <string.h>
#include <sys/param.h>
#include "syslib.h"


int sys_setrunnable(int pid) {
	message m;
	m.m_lc_pm_sig.pid = pid;
	return (_kernel_call(SYS_SETRUNNABLE, &m));

}
int sys_setblocked(int pid) {
	message m;
	m.m_lc_pm_sig.pid = pid;
	return(_kernel_call(SYS_SETBLOCKED, &m));
}

int sys_setsleep(int pid) {
//	printf("Inside kernel library call sys_setsleep\n");
	message m;
	int r;

	m.m_lc_pm_sig.pid = pid;
	r = _kernel_call(SYS_SETSLEEP, &m);
	return r;;
}


