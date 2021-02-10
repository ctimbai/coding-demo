#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include "process.h"

#define MAX_PATH_LEN 256
#define DIGIT_STRING "0123456789"

static int collect_single_process_info(int, struct process_state_t *);
static int collect_thread_info(int, int, struct process_state_t *);
static int get_proc_process_path(int, void *, size_t);
static int get_proc_thread_path(int, int, void *, size_t);
static int lookup_thread_id(const char *, int *, size_t);
static int lookup_pid_by_name(const char *);
static int read_process_state_fields(const char *, struct process_state_t *);
static int read_sys_prof(struct cpu_usage_t *);
static int collect_cpu_time(struct cpu_usage_t *, struct process_state_t *, int, int *, unsigned, unsigned);
static void print_process(const struct process_state_t *);
static void print_pid(const struct process_state_t *);
static void print_ppid(const struct process_state_t *);
static void print_process_time(const struct process_state_t *);
static void print_process_state(const struct process_state_t *);
static void print_process_stack(const struct process_state_t *);
static void print_process_signal(const struct process_state_t *);
static void print_vsize(const struct process_state_t *);
static void print_rss(const struct process_state_t *);
static void print_data_address(const struct process_state_t *);
static void print_cpu_usage(const struct cpu_usage_t *, const struct process_state_t *);
static void print_split_line();
static int compare(const void *, const void *);

int collect_process_info(struct arg_param *param, unsigned type)
{
    char path[MAX_PATH_LEN];
    int cur_len;

    if(param->interval == 0) {
        param->interval = 1;
    }
    if(param->pid == 0 && param->pname[0] == '\0') {
        fprintf(stderr, "Please specifice pid or process name at lease one. Use -h or --help for help.\n\n");
        return -1;
    }
    else if(param->pname[0] != 0 && param->pid == 0) {
        if((param->pid = lookup_pid_by_name(param->pname)) < 0) {
            fprintf(stderr, "lookup pid by name failed, no such process: %s\n", param->pname);
            return -1;
        }
    }
    int pid = param->pid;

    if((cur_len = get_proc_process_path(pid, path, sizeof(path))) < 0) {
        fprintf(stderr, "get process path at /proc failed. pid: %d\n", pid);
        return -1;
    }

    int id_rec[MAX_THREAD_SUPPORT + 1];

    strncat(path, PREFIX_TASK_PATH, sizeof(path) - cur_len);

    if(lookup_thread_id(path, id_rec, sizeof(id_rec)) < 0) {
        fprintf(stderr, "lookup thread id failed. path: %s\n", path);
        return -1;
    }

    if(param->sort) {
        int i, counter = 0;
        for(i = 0; id_rec[i]; i++) {
            counter++;
        }
        
        if(counter > 1) {
            qsort(id_rec, counter, sizeof(int), compare);
        }
    }
    
    struct process_state_t ps[MAX_THREAD_SUPPORT * 2 + 3];
    struct cpu_usage_t cu[2];

    if(collect_cpu_time(cu, ps, pid, id_rec, type, param->interval) < 0) {
        fprintf(stderr, "error occured when collecting cpu time.\n");
        return -1;
    }
    
    print_split_line();

    bool has_opt = false;

    if(type & e_cpu_usage) {
        print_cpu_usage(cu, ps);
        has_opt = true;
    }

    if(type & e_process_time) {
        print_process_time(ps + 2);
        has_opt = true;
    }

    if(!has_opt) {
        print_process(ps);
    }

    print_split_line();

    return 0;
}

static int collect_single_process_info(int pid, struct process_state_t *ps)
{
    char path[MAX_PATH_LEN];
    int cur_len;

    if((cur_len = get_proc_process_path(pid, path, sizeof(path))) < 0) {
        fprintf(stderr, "get thread path failed. pid: %d\n", pid);
        return -1;
    }

    strncat(path, PREFIX_STAT_FILE, sizeof(path) - cur_len);

    if(read_process_state_fields(path, ps) < 0) {
        fprintf(stderr, "read %s failed: %s\n", path, strerror(errno));
        return -1;
    }

    return 0;
}

static int collect_cpu_time(struct cpu_usage_t *cu, struct process_state_t *ps, int pid, int *id_rec, unsigned type, unsigned interval)
{
    if(read_sys_prof(cu) < 0) {
        fprintf(stderr, "collect cpu time failed");
        return -1;
    }
    if(collect_single_process_info(pid, ps) < 0) {
        fprintf(stderr, "collect process %d info failed.\n", pid);
        return -1;
    }
    int i; 
    for(i = 0; id_rec[i]; i++) {
        if(collect_thread_info(pid, id_rec[i], ps + i + 2) < 0) {
            fprintf(stderr, "collect process %d info failed.\n", pid);
            return -1;
        }
    }
    if(type & e_cpu_usage) {
        sleep(interval);
    }
    if(read_sys_prof(cu + 1) < 0) {
        fprintf(stderr, "collect cpu time failed");
        return -1;
    }
    if(collect_single_process_info(pid, ps + 1) < 0) {
        fprintf(stderr, "collect process %d info failed.\n", pid);
        return -1;
    }
    int last_idx = i;
    for(i = 0; id_rec[i]; i++) {
        if(collect_thread_info(pid, id_rec[i], ps + last_idx + i + 2) < 0) {
            fprintf(stderr, "collect process %d info failed.\n", pid);
            return -1;
        }
    }
    
    return 0;
}

static int collect_thread_info(int pid, int tid, struct process_state_t *ps)
{
    char path[MAX_PATH_LEN];
    int cur_len;

    if((cur_len = get_proc_thread_path(pid, tid, path, sizeof(path))) < 0) {
        fprintf(stderr, "get thread path failed. pid: %d, tid: %d\n", pid, tid);
        return -1;
    }

    strncat(path, PREFIX_STAT_FILE, sizeof(path) - cur_len);

    if(read_process_state_fields(path, ps) < 0) {
        fprintf(stderr, "read %s failed: %s\n", path, strerror(errno));
        return -1;
    }

    return 0;
}


static int get_proc_process_path(int pid, void *buf, size_t size)
{
    return snprintf(buf, size, PREFIX_PROC_PATH"%d", pid);
}

static int get_proc_thread_path(int pid, int tid, void *buf, size_t size)
{
    int cur_len;
    if((cur_len = get_proc_process_path(pid, buf, size)) < 0) {
        fprintf(stderr, "get process /proc path failed. pid: %d\n", pid);
        return -1;
    }

    size_t remain_size = size - cur_len - 1;  /* excluding null byte */
    
    if(remain_size <= strlen(PREFIX_TASK_PATH)) {
        fprintf(stderr, "there is no more buffer to store remain thread path. pid: %d, tid: %d\n", pid, tid);
        return -1;
    }
    void *remain_buf = buf + cur_len;

    return cur_len + snprintf(remain_buf, remain_size, PREFIX_TASK_PATH"%d", tid);
}

static int lookup_thread_id(const char *path, int *id_rec, size_t size)
{
    DIR *dp;
    if((dp = opendir(path)) == NULL) {
        fprintf(stderr, "opendir %s failed: %s\n", path, strerror(errno));
        return -1;
    }

    struct dirent *tdir;
    int i = 0;
    
    while((tdir = readdir(dp)) != NULL) {
        int tid_digit;
        char *end_ptr;
        if(i >= MAX_THREAD_SUPPORT) {
            fprintf(stderr, "Current thread number exceed max thread support %d.\n", MAX_THREAD_SUPPORT);
            return -1;
        }
        if(i < size - 1) {
            tid_digit = strtol(tdir->d_name, &end_ptr, 10);
            if(tid_digit && end_ptr[0] == '\0') {
                *id_rec++ = tid_digit;
            }
        } else {
            break;
        }
        i++;
    }
    *id_rec = 0;

    closedir(dp);

    return 0;
}

static int lookup_pid_by_name(const char *name)
{
    DIR *dp;
    char path[MAX_PATH_LEN];
    char comm[MAX_PATH_LEN];

    snprintf(path, sizeof(path), PREFIX_PROC_PATH);

    if((dp = opendir(path)) == NULL) {
        fprintf(stderr, "opendir %s failed: %s\n", path, strerror(errno));
        return -1;
    }

    struct dirent *dir;

    while((dir = readdir(dp)) != NULL) {
        if(strspn(dir->d_name, DIGIT_STRING) != strlen(dir->d_name)) {
            continue;
        }

        snprintf(comm, sizeof(comm), "%s%s"PREFIX_COMM_FILE, path, dir->d_name);

        FILE *fd;

        if((fd = fopen(comm, "r")) == NULL) {
            fprintf(stderr, "fopen %s failed: %s\n", comm, strerror(errno));
            continue;
        }

        char process_name[TASK_COMM_LEN];
        if(fscanf(fd, "%s", process_name) < 0) {
            fprintf(stderr, "fread %s failed: %s\n", comm, strerror(errno));
            continue;
        }

        if(strcmp(name, process_name) == 0) {
            closedir(dp);
            return (int)strtol(dir->d_name, NULL, 10);
        }
    }

    closedir(dp);

    return -1;
}

static int compare(const void *s1, const void *s2)
{
    return *(int *)s1 > *(int *)s2;
}

static int read_sys_prof(struct cpu_usage_t *cu)
{
    char cpuid[10];
    FILE *fd;

    if((fd = fopen(PREFIX_PROC_PATH"stat", "r")) == NULL) {
        fprintf(stderr, "fopen %s failed: %s\n", PREFIX_PROC_PATH"stat", strerror(errno));
        return -1;
    }

    if(fscanf(fd, "%s %lu%lu%lu%lu%lu%lu%lu%lu%lu%lu", cpuid, &cu->user, &cu->nice, &cu->system, &cu->idle, &cu->iowait, 
                &cu->irq, &cu->softirq, &cu->steal, &cu->guest, &cu->guest_nice) != 11) {
        fprintf(stderr, "fscanf %s failed: %s\n", PREFIX_PROC_PATH"stat", strerror(errno));
        return -1;
    }

    return 0;
}

static int read_process_state_fields(const char *path, struct process_state_t *ps)
{
    FILE *fd;
    
    if((fd = fopen(path, "r")) == NULL) {
        fprintf(stderr, "fopen %s failed: %s\n", path, strerror(errno));
        return -1;
    }

    int read_fields;

    if((read_fields = fscanf(fd, "%d %s %c %d%d%d%d%d"
               "%u%lu%lu%lu%lu%lu%lu"
               "%ld%ld%ld%ld%ld%ld"
               "%llu%lu%ld"
               "%lu%lu%lu%lu%lu%lu%lu"
               "%lu%lu%lu%lu%lu%lu"
               "%d%d%u%u%llu%lu%ld"
               "%lu%lu%lu%lu%lu%lu%lu%d",
               &ps->pid, ps->comm, &ps->state, &ps->ppid, &ps->pgrp, &ps->session, &ps->tty_nr, &ps->tpgid,
               &ps->flags, &ps->minflt, &ps->cminflt, &ps->majflt, &ps->cmajflt, &ps->utime, &ps->stime,
               &ps->cutime, &ps->cstime, &ps->priority, &ps->nice, &ps->num_threads, &ps->itrealvalue,
               &ps->starttime, &ps->vsize, &ps->rss, &ps->rsslim, &ps->startcode, &ps->endcode,
               &ps->startstack, &ps->kstkesp, &ps->kstkeip, &ps->signal, &ps->blocked, &ps->sigignore,
               &ps->sigcatch, &ps->wchan, &ps->nswap, &ps->cnswap, &ps->exit_signal, &ps->processor,
               &ps->rt_priority, &ps->policy, &ps->delayacct_blkio_ticks, &ps->guest_time, &ps->cguest_time,
               &ps->start_data, &ps->end_data, &ps->start_brk, &ps->arg_start, &ps->arg_end, &ps->env_start,
               &ps->env_end, &ps->exit_code)) != 52) {
        fprintf(stderr, "fscanf ret %d. %s failed: %s.\n", read_fields, path, strerror(errno));
        return -1;
    }

    fclose(fd);

    return 0;
}

static void print_process(const struct process_state_t *ps)
{
    print_pid(ps);
    print_ppid(ps);
    print_process_state(ps);
    print_rss(ps);
    print_vsize(ps);
    print_process_stack(ps);
    print_data_address(ps);
    print_process_signal(ps);
}

static void print_cpu_usage(const struct cpu_usage_t *cu, const struct process_state_t *ps)
{
    unsigned long cu1_time = cu->user + cu->nice + cu->system + cu->idle + cu->iowait + cu->irq + cu->softirq + cu->steal + cu->guest + cu->guest_nice;
    unsigned long cu2_time = (cu + 1)->user + (cu + 1)->nice + (cu + 1)->system + (cu + 1)->idle + (cu + 1)->iowait + (cu + 1)->irq + (cu + 1)->softirq + (cu + 1)->steal + (cu + 1)->guest + (cu + 1)->guest_nice;
    double cu_total = cu2_time - cu1_time;

    if(cu_total) {
        fprintf(stdout, "System wide: \n");
        fprintf(stdout, "User %.2f, sys %.2f, idle %.2f\n\n",
                ((cu + 1)->user - cu->user) / cu_total,
                ((cu + 1)->system - cu->system) / cu_total, 
                ((cu + 1)->idle - cu->idle) / cu_total);
        fprintf(stdout, "Process: \n");
        fprintf(stdout, "Pid %d: user %.2f, sys %.2f\n\n", (ps + 1)->pid, (((ps + 1)->utime + (ps + 1)->cutime) - (ps->utime + ps->cutime)) / cu_total, (((ps + 1)->stime + (ps + 1)->cstime) - (ps->stime + ps->cstime)) / cu_total);
        int i;
        int thread_num = (ps + 2)->num_threads;
        fprintf(stdout, "Num of threads: %d\n", thread_num);
        for(i = 0; i < thread_num; i++) {
            fprintf(stdout, "Tid %d%s: user %.2f, sys %.2f\n", (ps + i + 2)->pid, (ps + i + 2)->comm, ((ps + thread_num + i + 2)->utime - (ps + i + 2)->utime) / cu_total, ((ps + thread_num + i + 2)->stime - (ps + i + 2)->stime) / cu_total);
        }
    }
}

static void print_pid(const struct process_state_t *ps)
{
    fprintf(stdout, "Pid: %d\n", ps->pid);
}

static void print_ppid(const struct process_state_t *ps)
{
    fprintf(stdout, "Ppid: %d\n", ps->ppid);
}

static void print_process_time(const struct process_state_t *ps)
{
    int i, thread_num;
    thread_num = ps->num_threads;

    fprintf(stdout, "Num of threads: %d\n", thread_num);

    for(i = 0; i < thread_num; i++) {
        fprintf(stdout, "Thread %d%s: user %.3fs, sys %.3fs\n", (ps + i)->pid, (ps + i)->comm, (ps + i)->utime / (double)sysconf(_SC_CLK_TCK), (ps + i)->stime / (double)sysconf(_SC_CLK_TCK));
    }
}

static void print_process_state(const struct process_state_t *ps)
{
    fprintf(stdout, "Status: ");
    switch(ps->state) {
        case 'R':
            fprintf(stdout, "Running\n");
            break;
        case 'S':
            fprintf(stdout, "Sleeping\n");
            break;
        case 'D':
            fprintf(stdout, "Running\n");
            break;
        case 'Z':
            fprintf(stdout, "Zombie\n");
            break;
        case 'T':
            fprintf(stdout, "Stopped\n");
            break;
        case 't':
            fprintf(stdout, "Tracing stop\n");
            break;
        case 'W':
            fprintf(stdout, "Waking\n");
            break;
        case 'X':
        case 'x':
            fprintf(stdout, "Dead\n");
            break;
        case 'K':
            fprintf(stdout, "Wakekill\n");
            break;
        case 'P':
            fprintf(stdout, "Parked\n");
            break;
        default:
            fprintf(stdout, "Unknow option: %c\n", ps->state);
            break;
    }
}

static void print_process_stack(const struct process_state_t *ps)
{
    fprintf(stdout, "Start stack: %p\n", (void *)ps->startstack);
}

static void print_process_signal(const struct process_state_t *ps)
{
    fprintf(stdout, "Pending signal: %016lx\n", ps->signal);
    fprintf(stdout, "Blocked signal: %016lx\n", ps->blocked);
    fprintf(stdout, "Ignored signal: %016lx\n", ps->sigignore);
    fprintf(stdout, "Catched signal: %016lx\n", ps->sigcatch);
}

static void print_vsize(const struct process_state_t *ps)
{
    fprintf(stdout, "Virtual memory size: %lu KB\n", ps->vsize / 1024);
}

static void print_data_address(const struct process_state_t *ps)
{
    fprintf(stdout, "Start heap: %p\n", (void *)ps->start_brk);
    fprintf(stdout, "Start text: %p\n", (void *)ps->startcode);
    fprintf(stdout, "End text: %p\n", (void *)ps->endcode);
    fprintf(stdout, "Start data: %p\n", (void *)ps->start_data);
    fprintf(stdout, "End data: %p\n", (void *)ps->end_data);
}

static void print_rss(const struct process_state_t *ps)
{
    fprintf(stdout, "RSS: %ld\n", ps->rss);
}

static void print_split_line()
{
    fprintf(stdout, "+--------------------------------------------------------+\n");
}
