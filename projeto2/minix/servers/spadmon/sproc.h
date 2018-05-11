
EXTERN struct pstate {
  char state;
  int blocked;
  int pid;
  int rts_flag;
} pstate[NR_PROCS];

