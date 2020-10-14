/******************************************************************************
 *	File Name: server.c
 *	Author: 公众号: Linux云计算网络
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <poll.h>

#define IPADDRESS "127.0.0.1"
#define PORT 6666
#define MAXLINE 1024
#define LISTENQ 5
#define FDSIZE 1000
#define INFTIM -1

int socket_bind_listen(); 
int do_poll(int listenfd);

int main(int argc, char *argv[])
{
    int listenfd = socket_bind_listen();
    if (listenfd < 0) {
        return 0;
    }
    do_poll(listenfd);
    return 0;
}

int socket_bind_listen()
{
    int serverfd;
    struct sockaddr_in srv_addr;
    
    if ((serverfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket error!\n");
        return -1;
    }
    printf("socket ok!\n");
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(PORT);
    srv_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(srv_addr.sin_zero), 0);

    if (bind(serverfd, (struct sockaddr *)&srv_addr, 
                sizeof(struct sockaddr)) == -1) {
        perror("bind error!");
        return -2;
    }
    printf("bind ok!");
    
    if (listen(serverfd, LISTENQ) == -1) {
        perror("listen error!");
        return -3;
    }
    printf("listen ok!\n");

    return serverfd;
}

void do_poll(int listenfd) 
{
    int connfd, sockfd;
    struct sockaddr_in cliaddr;
    socklen_t cliaddrlen;
    struct pollfd clientfds[FDSIZE];
    int maxi;
    int i;
    int nready;

    clientfds[0].fd = listenfd;
    clientfds[0].events = POLLIN;

    for (i = 1; i < FDSIZE; ++i) {
        clientfds[i].fd = -1;
    }
    maxi = 0;
    while(1) {
        nready = poll(clientfds, maxi + 1, INFTIM);
        if (nready == -1) {
            perror("poll error!");
            exit(1);
        }
        // test listenfd is ready or not
        if (clientfds[0].revents & POLLIN) {
            cliaddrlen = sizeof(cliaddr);
            if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddrlen)) == -1) {
                if (errno == EINTR) {
                    continue;
                }
                else {
                    perror("accept error!");
                    exit(1);
                }
            }
            fprintf(stdout, "accept a new client: %s:%d\n", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);

            for (i = 1; i < FDSIZE; ++i) {
                if (clientfds[i].fd < 0) {
                    clientfds[i].fd = connfd;
                    break;
                }
            }
            if (i == FDSIZE) {
                fprintf(stdout, "too many clients.\n");
                exit(1);
            }

            // add new fd to poll events
            clientfds[i].events = POLLIN;

            // record the num of client socket
            maxi = (i > maxi ? i : maxi);
            if (--nready <= 0) {
                continue;
            }
        }

        // handle the packets from clients
        char buf[MAXLINE];
        memset(buf, 0, MAXLINE);
        int readlen = 0;
        for (i = 0; i <= maxi; ++i) {
            if (clientfds[i].fd < 0)
                continue;
            if (clientfds[i].revents & POLLIN) {
                readlen = read(clientfds[i].fd, buf, MAXLINE);
                if (readlen == 0) {
                    close(clientfds[i].fd);
                    clientfds[i].fd = -1;
                    continue;
                }
                write(STDOUT_FILENO, buf, readlen);
                write(clientfds[i].fd, buf, readlen);
            }
        }
    }
}
