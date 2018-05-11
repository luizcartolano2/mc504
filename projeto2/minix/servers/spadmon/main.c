/* This file contains the main program of the process manager and some related
 * procedures.  When MINIX starts up, the kernel runs for a little while,
 * initializing itself and its tasks, and then it runs PM and VFS.  Both PM
 * and VFS initialize themselves as far as they can. PM asks the kernel for
 * all free memory and starts serving requests.
 *
 * The entry points into this file are:
 *   main:      starts PM running
 *   reply:     send a reply to a process making a PM system call
 */

#include "spadmon.h"
#include <minix/callnr.h>
#include <minix/com.h>
#include <minix/ds.h>
#include <minix/endpoint.h>
#include <minix/minlib.h>
#include <minix/type.h>
#include <minix/vm.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <machine/archtypes.h>
#include <assert.h>
#include "mproc.h"

#include "kernel/const.h"
#include "kernel/config.h"
#include "kernel/proc.h"

#if ENABLE_SYSCALL_STATS
EXTERN unsigned long calls_stats[NR_SPADMON_CALLS];
#endif

static int get_nice_value(int queue);

/* SEF functions and variables. */
static void sef_local_startup(void);
static int sef_cb_init_fresh(int type, sef_init_info_t *info);

/*===========================================================================*
 *                              main                                         *
 *===========================================================================*/
int
main(void)
{
/* Main routine of the process manager. */
  unsigned int call_index;
  int ipc_status, result;

  /* SEF local startup. */
  sef_local_startup();

  /* This is PM's main loop-  get work and do it, forever and forever. */
  while (TRUE) {
        /* Wait for the next message. */
        if (sef_receive_status(ANY, &m_in, &ipc_status) != OK)
                panic("PM sef_receive_status error");

        /* Extract useful information from the message. */
        who_e = m_in.m_source;  /* who sent the message */
        who_p = _ENDPOINT_P(m_in.m_source);

        call_nr = m_in.m_type;  /* system call number */

  if (IS_SPADMON_CALL(call_nr)) {
                /* If the system call number is valid, perform the call. */
                call_index = (unsigned int) (call_nr - SPADMON_BASE);

                if (call_index < NR_SPADMON_CALLS && call_vec[call_index] != NULL) {
                        result = (*call_vec[call_index])();
                } else
                        result = ENOSYS;
        } else
                result = ENOSYS;

        /* Send reply. */
        if (result != SUSPEND) reply(who_e, who_p, result);
  }
  return(OK);
}

/*===========================================================================*
 *                             sef_local_startup                             *
 *===========================================================================*/
static void
sef_local_startup(void)
{
  /* Register init callbacks. */
  sef_setcb_init_fresh(sef_cb_init_fresh);
  sef_setcb_init_restart(SEF_CB_INIT_RESTART_STATEFUL);

  /* Let SEF perform startup. */
  sef_startup();
}

/*===========================================================================*
 *                              reply                                        *
 *===========================================================================*/
void
reply(
        int dest_e,                     /* endpoint of process to reply to */
        int proc_nr,                    /* process to reply to */
        int result                      /* result of call (usually OK or error #) 
*/
)
{
/* Send a reply to a user process.  System calls may occasionally fill in other
 * fields, this is only for the main return value and for sending the reply.
 */
  struct mproc *rmp;
  int r;

  if(proc_nr < 0 || proc_nr >= NR_PROCS)
      panic("reply arg out of range: %d", proc_nr);

  rmp = &mproc[proc_nr];
  rmp->mp_reply.m_type = result;

  if ((r = ipc_sendnb(dest_e, &rmp->mp_reply)) != OK)
        printf("SPADMON can't reply to %d (%s): %d\n", rmp->mp_endpoint,
                rmp->mp_name, r);
}



/*===========================================================================*
 *                          sef_cb_init_fresh                                *
 *===========================================================================*/
static int sef_cb_init_fresh(int UNUSED(type), sef_init_info_t *UNUSED(info))
{
  /*No need to setup anything for spadmon.*/
  return(OK);
}
