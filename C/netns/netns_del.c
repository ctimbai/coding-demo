#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mount.h>
#include <errno.h>
#include <string.h>


int main(int argc, char **argv) {
    char *name = argv[1];

    if (argc < 2) {
        printf("Usage: %s netns_name\n", argv[0]);
        return -1;
    }

    // 对于绑定挂载的文件，需要先umount
    if (umount2(name, MNT_DETACH)) {
        // 出错不影响继续
    }

    // umount该文件与network namespace不再有关系，删除
    if (unlink(name) < 0) {
        fprintf(stderr, "Cannot remove namespace file \"%s\": %s\n",
                name, strerror(errno));
        return -1;
    }

    return 0;
}