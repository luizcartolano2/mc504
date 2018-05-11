#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <lib.h>
#include "/usr/src/minix/servers/pm/mproc.h"
#include "/usr/src/minix/servers/spadmon/sproc.h"
#include "/usr/src/minix/include/minix/com.h"
#include "/usr/src/minix/include/minix/callnr.h"

#include <minix/sysinfo.h>

#define INVARG  1

#define HELP            0
#define PS              1
#define RUN             2
#define SLEEP           3
#define STOP            4
#define ZOMBIES         5
#define EXIT            6

struct args {
        int flags[8];
        int pid;
};

/* helpfull functions */
char checkProcState (int pid);

/* Parsing functions */
int parseCmd(int argc, char **argv, struct args *options);
void parseOpt(char *opt, struct args *options);
void isValidPos(struct args *options);
int getOption(struct args *options);
int getMaxDigits(struct pstate *ptable);

/* Functions that actually do the work */
void printHelp(int errn);
void printProcState(void);
void setRunnable(int pid, int verbose);
void setSleep(int pid, int verbose);
void setStopped(int pid, int verbose);
void setZombie(int pid, int verbose);
void setExit(int pid, int verbose);

int main(int argc, char** argv) {
        struct args *options = (struct args*) malloc(sizeof(struct args));
        int cmd = -1, v = 0;

        cmd = parseCmd(argc, argv, options);
        v = options->flags[7];
        
        switch(cmd) {
                case HELP:
                        printHelp(0);
                        break;
                case PS:
                        printProcState();
                        break;
                case RUN:
                        setRunnable(options->pid, v);
                        break;
                case SLEEP:
                        setSleep(options->pid, v);
                        break;
                case STOP:
                        setStopped(options->pid, v);
                        break;
                case ZOMBIES:
                        setZombie(options->pid, v);
                        break;
                case EXIT:
                        setExit(options->pid, v);
                        break;
                default:
                        printHelp(INVARG);
        }
                return 0;
}

void printProcState(void) {
        struct pstate *rmp;
        struct pstate *ptable = (struct pstate *) malloc(sizeof(struct pstate) * NR_PROCS);
        int blocked;
	int digits;
	int r;
	
		/*Syscall that retreives process data*/

	//r = get_pstatus(ptable);
	
	message m;
	m.m_lsys_getsysinfo.what = SI_PROC_TAB;
	m.m_lsys_getsysinfo.where = (vir_bytes) ptable;
  	m.m_lsys_getsysinfo.size = sizeof(struct pstate) * NR_PROCS;
  	r = _syscall(SPADMON_PROC_NR, SPADMON_GETPTABLE, &m);


	if (r) {
	        printf("ERROR: Spadmon server failed to retrieve process tables.\n");
        }
	
		/*Gets maximum number of digits*/
		digits = getMaxDigits(ptable);
		
        printf("----------------------\n"\
               " %-15s%-6s\n"\
               "----------------------\n", "PID/EndPoint", "State");
        for (rmp = &ptable[0]; rmp < &ptable[NR_PROCS]; rmp++) {
                blocked = rmp->blocked;
                if (blocked != 0 && blocked != 1 && blocked != 31743 && blocked != 31744) 
                        blocked = 1;
                else
                        blocked = 0;
                if (rmp->state != 'U') {
                        printf(" %0*d%*s%c\n", digits, rmp->pid, 15-digits, " ",  blocked ? 'D': rmp->state);
                }
        }
        printf("----------------------\n");
}

void printHelp(int errn) {
        const char* help = "Usage: padmon [OPTION]... [PID/ENDPOINT]...\n"\
                "Choose one of the options below. Exactly one of these arguments is mandatory:\n"\
                "  -help                 Show this help message and exit.\n"\
                "  -ps                   Prints all processes' PID and state.\n"\
                "  -r <pid/endpoint>     Change process <pid/endpoint> to running or runnable.\n"\
                "  -s <pid/endpoint>     Change process <pid/endpoint> to interruptible sleep.\n"\
                "  -t <pid/endpoint>     Change process <pid/endpoint> to stopped.\n"\
                "  -z <pid/endpoint>     Change process <pid/endpoint> to zombie.\n"\
                "  -e <pid/endpoint>     Exit process <pid/endpoint>.\n\n"
                "  Optionally display more info:\n"\
                "  -v                    Verbosely display command execution.\n";
        printf("%s", help);
}

void setRunnable(int pid, int verbose) {
        int r;
        char check;

        if ((check = checkProcState(pid)) == INVARG) {
                        printf("ERROR: A process with PID %d doesn't exist.\n", pid);
                        return;
                }

        if (check == 'Z') {
                printf("ERROR: Can't change the state of a zombie process.\n");
                return;
        } 
                else if (check == 'D') {
                printf("ERROR: Can't change the state of an uninterruptible process.\n");
                return;
        }
                else if (check == 'R') {
                                if (verbose) printf("Process with PID %d is already running.\n", pid);
                                return;
                }

	//r = set_runnable(pid);
	message m;
  	memset(&m, 0, sizeof(m));
  	m.m_lc_pm_sig.pid = pid;
	r = _syscall(SPADMON_PROC_NR, SPADMON_SETRUNNABLE, &m);


                if (r) {
                        printf("ERROR: Spadmon server failed to change process with PID %d to running.\n", pid);
                }
                else {
                        if(verbose) printf("SUCCESS: Set process with PID %d to running.\n", pid);
                }
}

void setSleep(int pid, int verbose) {
        int r;
        char check;

        if ((check = checkProcState(pid)) == INVARG) {
                        printf("ERROR: A process with PID %d doesn't exist.\n", pid);
                        return;
                }

        if (check == 'Z') {
                        printf("ERROR: Can't change the state of a zombie process.\n");
                        return;

        } 
                else if (check == 'D') {
            printf("ERROR: Can't change the state of an uninterruptible process.\n");
            return;
        }
                else if (check == 'S') {
                        if (verbose) printf("Process with PID %d is already asleep.\n", pid);
                        return;
                }
                        
	// r = set_sleep(pid);
	message m;
	memset(&m, 0, sizeof(m));
  	m.m_lc_pm_sig.pid = pid;
  	r = _syscall(SPADMON_PROC_NR, SPADMON_SETSLEEP, &m);

        
                if (r) {
                        printf("ERROR: Spadmon server failed to change process with PID %d to sleep.\n", pid);
                }
                else {
                        if (verbose) printf("SUCCESS: Set process %d to sleep.\n", pid);
                }
}

void setStopped(int pid, int verbose) {
        int r;
        char check;

        if ((check = checkProcState(pid)) == INVARG) {
                        printf("ERROR: A process with PID %d doesn't exist.\n", pid);
                        return;
                }

                if (check == 'Z') {
                printf("ERROR: Can't change the state of a zombie process.\n");
                return;
                } 
                else if (check == 'D') {
                printf("ERROR: Can't change the state of an uninterruptible process.\n");
                return;
        }
                else if (check == 'T') {
                        if (verbose) printf("Process with PID %d is already stopped\n", pid);
                        return;
                }

	//r = set_stopped(pid); 
  	message m;
	memset(&m, 0, sizeof(m));
	m.m_lc_pm_sig.pid = pid;
  	r = _syscall(SPADMON_PROC_NR, SPADMON_SETSTOPPED, &m);

                if (r) {
                        printf("ERROR: Spadmon server failed to change process with PID %d to uninterruptible.\n", pid);
                }
                else {
                        if (verbose) printf("SUCCESS: Set process with PID %d to stopped\n", pid);
                }
                return;
}

void setZombie(int pid, int verbose) {
        int r;
        char check;

        if ((check = checkProcState(pid)) == INVARG) {
                        printf("ERROR: A process with PID %d doesn't exist.\n", pid);
                        return;
                }

        if (check == 'D') {
                printf("ERROR: Can't change the state of an uninterruptible process.\n");
                return;
        }
                else if (check == 'Z') {
                                if (verbose) printf("Process with PID %d is already a zombie.\n", pid);
                                return;
                }

        //r = set_zombie(pid);
	message m;
	memset(&m, 0, sizeof(m));
  	m.m_lc_pm_sig.pid = pid;
  	r = _syscall(SPADMON_PROC_NR, SPADMON_SETZOMBIE, &m);

                if (r) {
                        printf("ERROR: Spadmon server failed to change process with PID %d to zombie.\n", pid);
                }
                else {
                        if(verbose) printf("SUCCESS: Set process with PID %d to zombie\n", pid);
        }
}

void setExit(int pid, int verbose) {
        int r;
	char check;

        if ((check = checkProcState(pid)) == INVARG) {
                        if (verbose) printf("Process with PID %d doesn't exist or has already terminated.\n", pid);
                        return;
                }

       	if (check == 'Z') {
                printf("ERROR: Can't exit a zombie process.\n");
        	return;
        }

        //r = set_exit(pid);
  	message m;
	memset(&m, 0, sizeof(m));
	m.m_lc_pm_sig.pid = pid;
  	r = _syscall(SPADMON_PROC_NR, SPADMON_SETEXIT, &m);


                if (r) {
                        printf("ERROR: Spadmon failed to exit process with PID %d\n", pid);
                }
                else {
                if (verbose) printf("SUCCESS: Exited process with PID %d\n", pid);
        }
}

int getMaxDigits(struct pstate *ptable) {
	struct pstate *rmp;
	int maxPID = -1;

	for (rmp = &ptable[0]; rmp < &ptable[NR_PROCS]; rmp++) {
		if (rmp->pid > maxPID)
			maxPID = rmp->pid;
	}

	/* We assume no PID will get into the millions */
	if (maxPID < 10) return 1;
	if (maxPID < 100) return 2;
	if (maxPID < 1000) return 3;
	if (maxPID < 10000) return 4;
	if (maxPID < 100000) return 5;
	if (maxPID < 1000000) return 6;
	
	return 0;
}

int parseCmd(int argc, char **argv, struct args *options) {
        int i = 0;
        int pid = 0;
        char *opt = NULL;

        if (argc < 2 || argc > 4) {
                printHelp(INVARG);
        }
        for (i = 1; i < argc; i++) {
                if ((opt = strchr(argv[i], '-')) != NULL) {
                        parseOpt(&opt[1], options);
                }
                else if ((pid = atoi(argv[i]))) {
                        options->pid = pid;
                        isValidPos(options);
                }
                else {
                        printHelp(INVARG);
                }
        }
        return getOption(options);
}

void parseOpt(char *opt, struct args *options) {
        if (strcmp(opt, "help") == 0) {
                options->flags[0] = 1;
                return;
        }
        else if (strcmp(opt, "ps") == 0) {
                options->flags[1] = 1;
                return;
        }
        else if (strcmp(opt, "r") == 0) {
                options->flags[2] = 1;
                return;
        }
        else if (strcmp(opt, "s") == 0) {
                options->flags[3] = 1;
                return;
        }
        else if (strcmp(opt, "t") == 0) {
                options->flags[4] = 1;
                return;
        }
        else if (strcmp(opt, "z") == 0) {
                options->flags[5] = 1;
                return;
        }
        else if (strcmp(opt, "e") == 0) {
                options->flags[6] = 1;
                return;
        }
        else if (strcmp(opt, "v") == 0) {
                options->flags[7] = 1;
                return;
        }
        else if (strcmp(opt, "rv") == 0 || strcmp(opt, "vr") == 0) {
                options->flags[2] = 1;
                options->flags[7] = 1;
                return;
        }
        else if (strcmp(opt, "sv") == 0 || strcmp(opt, "vs") == 0) {
                options->flags[3] = 1;
                options->flags[7] = 1;
                return;
        }
        else if (strcmp(opt, "tv") == 0 || strcmp(opt, "vt") == 0) {
                options->flags[4] = 1;
                options->flags[7] = 1;
                return;
        }
        else if (strcmp(opt, "zv") == 0 || strcmp(opt, "vz") == 0) {
                options->flags[5] = 1;
                options->flags[7] = 1;
                return;
        }
        else if (strcmp(opt, "ev") == 0 || strcmp(opt, "ve") == 0) {
                options->flags[6] = 1;
                options->flags[7] = 1;
                return;
        }
        else {
                printHelp(INVARG);
        }
}

int getOption(struct args *options) {
        int opt_id = -1;
        int opt_count = 0;

        for (int i = 0; i < 7; i++) {
                        if(options->flags[i]) {
                                opt_count++;
                                opt_id = i;
                        }
        }

        if (opt_count != 1) {
                printHelp(INVARG);
        }
        return opt_id;
}

void isValidPos(struct args *options) {
        if (!options->flags[2] && !options->flags[3] && !options->flags[4] && 
!options->flags[5] && !options->flags[6]) {
                printHelp(INVARG);
        }
}

char checkProcState (int pid) {
        struct pstate *rmp;
        struct pstate *ptable = (struct pstate *) malloc(sizeof(struct pstate) * NR_PROCS);
        int blocked;
        char state;

        get_pstatus(ptable);


        for (rmp = &ptable[0]; rmp < &ptable[NR_PROCS]; rmp++) {
                blocked = rmp->blocked;
                if (blocked != 0 && blocked != 1 && blocked != 31743 && blocked != 31744)
                        blocked = 1;
                else
                        blocked = 0;
                if (rmp->state != 'U') {
                        if (rmp->pid == pid) {
                                                                state = blocked ? 'D' : rmp->state;
                                return state;
                        }
                }
        }
        return INVARG;
}

