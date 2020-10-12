/******************************************************************************
 *	File Name: client.c
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

#define DEFAULT_PORT 6666
#define MAX_BUFFER 1024

int main(int argc, char *argv[])
{
    int connfd = 0;
    int len = 0;
    struct sockaddr_in cli;
    if (argc < 2) {
        printf("Usage: client [server IP address]\n");
        return -1;
    }
    
    cli.sin_family = AF_INET;
    cli.sin_port = htons(DEFAULT_PORT);
    cli.sin_addr.s_addr = inet_addr(argv[1]);
    
    connfd = socket(AF_INET, SOCK_STREAM, 0);
    if (connfd <  0) {
        perror("socket error!\n");
        return -1;
    }

    if (connect(connfd, (struct sockaddr *)&cli, sizeof(cli)) < 0) {
        perror("connect error!\n");
        return -1;
    }

    char buffer[MAX_BUFFER];
    bzero(buffer, sizeof(buffer));

    recv(connfd, buffer, MAX_BUFFER, 0);
    printf("recv: %s\n", buffer);

    bzero(buffer, sizeof(buffer));
    strcpy(buffer, "this is client!\n");
    send(connfd, buffer, MAX_BUFFER, 0);
    while(1) {
        bzero(buffer, sizeof(buffer));
        scanf("%s", buffer);
        buffer[strlen(buffer)] = '\0';
        send(connfd, buffer, MAX_BUFFER, 0);
        printf("I have send buffer\n");
    }

    close(connfd);
    return 0;
}
