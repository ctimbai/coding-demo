#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <errno.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    char *name = argv[1];
    int fd;

    if (argc < 2) {
        printf("Usage: %s netns_name\n", argv[0]);
        return -1;
    }

    // 使用unshare将进程的network namespace剥离，创建一个新的network namespace
    if (unshare(CLONE_NEWNET) < 0) {
        fprintf(stderr, "Failed to create a new network namespace \"%s\": %s\n",
                name, strerror(errno));
        return -1;
    }

    fd = open(name, O_RDONLY|O_CREAT|O_EXCL, 0);
    if (fd < 0) {
        fprintf(stderr, "Cannot create namespace file \"%s\": %s\n",
                name, strerror(errno));
        return -1;
    }
    close(fd);

    // 使用绑定挂载保持新network namespace在进程退出后依然存在，
    // 并使用文件名作为该network namespace的标识。
    // 为什么绑定挂载可以达到这个目的，可能与proc文件系统中ns实现有关，这里不做关注
    if (mount("/proc/self/ns/net", name, "none", MS_BIND, NULL) < 0) {
        fprintf(stderr, "Bind /proc/self/ns/net -> %s failed: %s\n",
                name, strerror(errno));
        return -1;
    }

    return 0;
}