/* Function prototypes. */

struct mproc;

#include <minix/timers.h>

/* get_table.c */
int do_getptable(void);

/* forward.c */
int do_setrunnable(void);
int do_setsleep(void);
int do_setstopped(void);
int do_setzombie(void);
int do_setexit(void);

/* main.c */
int main(void);
void reply(int dest_e, int proc_nr, int result);
