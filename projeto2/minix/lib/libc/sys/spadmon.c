#include <sys/cdefs.h>
#include "namespace.h"
#include <lib.h>

#include <string.h>
#include <unistd.h>
#include "/usr/src/minix/servers/pm/mproc.h"
#include "/usr/src/minix/servers/spadmon/sproc.h"
#include <minix/config.h>
#include <minix/sysinfo.h>

int get_pstatus(void *pstate)
{
  message m;

  memset(&m, 0, sizeof(m));
  m.m_lsys_getsysinfo.what = SI_PROC_TAB;
  m.m_lsys_getsysinfo.where = (vir_bytes) pstate;
  m.m_lsys_getsysinfo.size = sizeof(struct pstate) * NR_PROCS; 
  return (_syscall(SPADMON_PROC_NR, SPADMON_GETPTABLE, &m));
}

int set_runnable(pid_t proc)
{
  message m;

  memset(&m, 0, sizeof(m));
  m.m_lc_pm_sig.pid = proc;
  return(_syscall(SPADMON_PROC_NR, SPADMON_SETRUNNABLE, &m));
}

int set_sleep(pid_t proc)
{
  message m;

  memset(&m, 0, sizeof(m));
  m.m_lc_pm_sig.pid = proc;
  return(_syscall(SPADMON_PROC_NR, SPADMON_SETSLEEP, &m));
}

int set_stopped(pid_t proc)
{
  message m;

  memset(&m, 0, sizeof(m));
  m.m_lc_pm_sig.pid = proc;
  return(_syscall(SPADMON_PROC_NR, SPADMON_SETSTOPPED, &m));
}

int set_zombie(pid_t proc)
{
  message m;

  memset(&m, 0, sizeof(m));
  m.m_lc_pm_sig.pid = proc;
  return(_syscall(SPADMON_PROC_NR, SPADMON_SETZOMBIE, &m));
}

int set_exit(pid_t proc)
{
  message m;

  memset(&m, 0, sizeof(m));
  m.m_lc_pm_sig.pid = proc;
  return(_syscall(SPADMON_PROC_NR, SPADMON_SETEXIT, &m));
}



