#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main()
{
    int keyboard;
    int ret, i;
    char c;

    fd_set readfd;
    struct timeval timeout;

    keyboard = open("/dev/tty", O_RDONLY | O_NONBLOCK);
    assert(keyboard > 0);

    while(1) {
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        FD_ZERO(&readfd);
        FD_SET(keyboard, &readfd);
        ret = select(keyboard + 1, &readfd, NULL, NULL, &timeout);

        if (FD_ISSET(keyboard, &readfd)) {
            i = read(keyboard, &c, 1);
            if ('\n' == c) {
                continue;
            }
            printf("input is: %c\n", c);
            if ('q' == c) {
                break;
            }
        }
    }
    return 0;
}
