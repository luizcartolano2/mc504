/* This file contains the table used to map system call numbers onto the
 * routines that perform them.
 */

#define _TABLE

#include "spadmon.h"
#include <minix/callnr.h>
#include <signal.h>
#include "mproc.h"
#include "sproc.h"

#define CALL(n) [((n) - SPADMON_BASE)]

int (* const call_vec[NR_SPADMON_CALLS])(void) = {
        CALL(SPADMON_GETPTABLE)      = do_getptable,         /* get PM's process table */
        CALL(SPADMON_SETRUNNABLE)    = do_setrunnable,       /* set process to runnable */
        CALL(SPADMON_SETSLEEP)       = do_setsleep,          /* set process to sleep */
        CALL(SPADMON_SETSTOPPED)     = do_setstopped,        /* set process to stopped */
        CALL(SPADMON_SETZOMBIE)      = do_setzombie,         /* set process to zombie */
        CALL(SPADMON_SETEXIT)        = do_setexit            /* exit process */
};
