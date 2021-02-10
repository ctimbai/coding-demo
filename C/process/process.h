#ifndef PROCESS_H
#define PROCESS_H

#define TASK_COMM_LEN 18
#define PREFIX_PROC_PATH "/proc/"
#define PREFIX_TASK_PATH "/task/"
#define PREFIX_STAT_FILE "/stat"
#define PREFIX_COMM_FILE "/comm"

struct process_state_t {
    int pid;  /* Process id */
    char comm[TASK_COMM_LEN];  /* Filename of the executable */
    char state;  /* Process state */
    int ppid;  /* The pid of the parent of this process */
    int pgrp;  /* The process group id of this process */
    int session;  /* The session id of this process */
    int tty_nr;  /* The controlling terminal of the process */
    int tpgid;  /* The ID of the foreground process group of the controlling terminal of the process */
    unsigned flags;  /* The kernel flags word of the process */
    unsigned long minflt;
    unsigned long cminflt;
    unsigned long majflt;
    unsigned long cmajflt;
    unsigned long utime;  /* Amount of time that this process has been scheduled in user mode */
    unsigned long stime;  /* Amount of time that this process has been scheduled in kernel mode */
    long cutime;  /* Amount  of  time that this process's waited-for children have been scheduled in user mode */
    long cstime;  /* Amount of time that this process's waited-for children have been scheduled in kernel mode */
    long priority;  /* For processes running a real-time scheduling policy */
    long nice;  /* -20 - 19 */
    long num_threads;  /* Number of threads in this process */
    long itrealvalue;  /* The time in jiffies before the next SIGALRM is sent to the process due to an interval timer */
    unsigned long long starttime;  /* The time the process started after system boot */
    unsigned long vsize;  /* Virtual memory size in bytes */
    long rss;  /* Resident Set Size: number of pages the process has in real memory */
    unsigned long rsslim;  /* Current soft limit in bytes on the rss of the process */
    unsigned long startcode;  /* The address above which program text can run */
    unsigned long endcode;  /* The address below which program text can run */
    unsigned long startstack;  /* The address of the start (i.e., bottom) of the stack */
    unsigned long kstkesp;  /* The current value of ESP (stack pointer) */
    unsigned long kstkeip;  /* The current EIP (instruction pointer) */
    unsigned long signal;  /* The bitmap of pending signals, displayed as a decimal number */
    unsigned long blocked;  /* The bitmap of blocked signals, displayed as a decimal number */
    unsigned long sigignore;  /* The bitmap of ignored signals, displayed as a decimal number */
    unsigned long sigcatch;  /* The bitmap of caught signals, displayed as a decimal number */
    unsigned long wchan;  /* This is the "channel" in which the process is waiting */
    unsigned long nswap;  /* Number of pages swapped (not maintained) */
    unsigned long cnswap;  /* Cumulative nswap for child processes (not maintained) */
    int exit_signal;  /* Signal to be sent to parent when we die */
    int processor;  /*  CPU number last executed on */
    unsigned rt_priority;  /* Real-time scheduling priority */
    unsigned policy;  /* Scheduling policy */
    unsigned long long delayacct_blkio_ticks;  /* Aggregated block I/O delays, measured in clock ticks (centiseconds) */
    unsigned long guest_time;  /* Guest time of the process (time spent running a virtual CPU for a guest operating system) */
    long cguest_time;  /* Guest time of the process's children */
    unsigned long start_data;  /* Address above which program initialized and uninitialized (BSS) data are placed */
    unsigned long end_data;  /* Address below which program initialized and uninitialized (BSS) data are placed */
    unsigned long start_brk;  /* Address above which program heap can be expanded with brk() */
    unsigned long arg_start;  /* Address above which program command-line arguments (argv) are placed */
    unsigned long arg_end;  /* Address below program command-line arguments (argv) are placed */
    unsigned long env_start;  /* Address above which program environment is placed */
    unsigned long env_end;  /* Address below which program environment is place */
    int exit_code;  /* The thread's exit status in the form reported by waitpid() */
};

struct cpu_usage_t {
    unsigned long user;
    unsigned long nice;
    unsigned long system;
    unsigned long idle;
    unsigned long iowait;
    unsigned long irq;
    unsigned long softirq;
    unsigned long steal;
    unsigned long guest;
    unsigned long guest_nice;
};

struct arg_param {
    int interval;
    int pid;
    int sort;
    char pname[32];
};

enum print_type {
    e_cpu_usage = 0x01,
    e_process_time = 0x02,
};

extern int collect_process_info(struct arg_param *, unsigned);

#endif
