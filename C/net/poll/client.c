/******************************************************************************
 *  File Name: server.c
 *  Author: 公众号: Linux云计算网络
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

#define PORT 6666
#define MAXLINE 1024

#define max(a, b) (a > b)?(a):(b)

void handle_connection(int sockfd);

int main(int argc, char *argv[])
{
    int connfd = 0;
    int len = 0;
    struct sockaddr_in cliaddr;

    if (argc < 2) {
        printf("Usage: client [server IP address]\n");
        return -1;
    }

    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(PORT);
    cliaddr.sin_addr.s_addr = inet_addr(argv[1]);

    if ((connfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket error!\n");
        return -1;
    }
    printf("socket ok!\n");

    if (connect(connfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr)) < 0) {
        perror("connect error!\n");
        return -1;
    }

    handle_connection(connfd);
    return 0;
}

void handle_connection(int sockfd)
{
    char sendline[MAXLINE], recvline[MAXLINE];
    int maxfdp, stdineof;
    struct pollfd pfds[2];
    int n;

    // add connfd
    pfds[0].fd = sockfd;
    pfds[0].events = POLLIN;

    // add stdin fd
    pfds[1].fd = STDIN_FILENO;
    pfds[1].events = POLLIN;

    while (1) {
        poll(pfds, 2, -1);
        if (pfds[0].revents & POLLIN) {
            n = read(sockfd, recvline, MAXLINE);
            if (n == 0) {
                fprintf(stderr, "client: server is closed!\n");
                close(sockfd);
            }
            write(STDOUT_FILENO, recvline, n);
        }
        if (pfds[1].revents & POLLIN) {
            n = read(STDIN_FILENO, sendline, MAXLINE);
            if (n == 0) {
                shutdown(sockfd, SHUT_WR);
                continue;
            }
            write(sockfd, sendline, n);
        }
    }
}