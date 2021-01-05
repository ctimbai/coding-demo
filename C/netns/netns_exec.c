#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sched.h>
#include <unistd.h>

int netns_switch(char *name)
{
    int netns;

    netns = open(name, O_RDONLY | O_CLOEXEC);
    if (netns < 0) {
        fprintf(stderr, "Cannot open network namespace \"%s\": %s\n",
                name, strerror(errno));
        return -1;
    }

    // 使用setns切换到指定的network namespace
    if (setns(netns, CLONE_NEWNET) < 0) {
        fprintf(stderr, "setting the network namespace \"%s\" failed: %s\n",
                name, strerror(errno));
        close(netns);
        return -1;
    }
    close(netns);

    // 处理mount

    // 处理/etc下配置文件

    return 0;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Usage: %s netns_name cmd\n", argv[0]);
        return -1;
    }

    // 使用setns切换到指定的network namespace
    if (netns_switch(argv[1])) {
        return -1;
    }

    // 进程已经切换到指定network namespace，执行参数指定的可执行程序
    if (execvp(argv[2], &argv[2])  < 0) {
        fprintf(stderr, "exec of \"%s\" failed: %s\n",
                argv[2], strerror(errno));
        return -1;
    }

    return 0;
}