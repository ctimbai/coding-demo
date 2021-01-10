/******************************************************************************
 *	File Name: select.c
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

#define DEFAULT_PORT 6666
#define MAX_LISTEN_NUM 10
#define MAX_LIVE_FD_NUM 5
#define MAX_BUFFER 1024

int main(int argc, char **argv)
{
    int serverfd; // listenning
    int acceptfd; // proccesing
    struct sockaddr_in srv_addr; // server addr 
    unsigned int listen_port = DEFAULT_PORT;
    unsigned int nsize;

    if ((serverfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket error!");
        return -1;
    }
    printf("socket ok!\n");
    
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(DEFAULT_PORT);
    srv_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(srv_addr.sin_zero), 0);

    if (bind(serverfd, (struct sockaddr *)&srv_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind error!");
        return -2;
    }
    printf("bind ok!\n");

    if (listen(serverfd, MAX_LISTEN_NUM) == -1) {
        perror("listen error!");
        return -3;
    }
    printf("listen ok!\n");

    fd_set client_fdset;
    int maxfd;
    struct timeval tv;
    int client_sockfd[MAX_LIVE_FD_NUM]; // record the live client fd
    bzero((void *)client_sockfd, sizeof(client_sockfd));
    int conn_amount = 0;
    maxfd = serverfd; // init the max maxfd
    char buffer[MAX_BUFFER];
    int ret;

    while(1) {
        FD_ZERO(&client_fdset);
        FD_SET(serverfd, &client_fdset);

        tv.tv_sec = 30; // 30s
        tv.tv_usec = 0;

        for (int i = 0; i < MAX_LIVE_FD_NUM; i ++) {
            if (client_sockfd[i] != 0) {
                FD_SET(client_sockfd[i], &client_fdset);
            }
        }

        // select
        ret = select(maxfd + 1, &client_fdset, NULL, NULL, &tv);
        if (ret < 0) {
            perror("select error!");
            break;
        }
        else if (ret == 0) {
            printf("timeout!\n");
            continue; // not break
        }
        // poll the fdset
        for (int i = 0; i < conn_amount; ++i) {
            if (FD_ISSET(client_sockfd[i], &client_fdset)) {
                printf("start recv from client[%d]:\n", i);
                ret = recv(client_sockfd[i], buffer, MAX_BUFFER, 0);
                if (ret <= 0) {
                    printf("client[%d] close\n", i);
                    close(client_sockfd[i]);
                    FD_CLR(client_sockfd[i], &client_fdset);
                    client_sockfd[i] = 0;
                }
                else {
                    printf("recv from client[%d]: %s\n", i, buffer);
                }
            }
        }
        // check there is new connection or not, if yes, put it to client_sockfd
        if (FD_ISSET(serverfd, &client_fdset)) {
            struct sockaddr_in cli_addr; // client addr
            size_t size = sizeof(struct sockaddr_in);
            int clifd = accept(serverfd, (struct sockaddr *)(&cli_addr), 
                    (unsigned int *)(&size));
            if (clifd < 0) {
                perror("accept error!");
                continue; // not break
            }
            if (conn_amount < MAX_LIVE_FD_NUM) {
                client_sockfd[conn_amount++] = clifd;
                bzero(buffer, MAX_BUFFER);
                strcpy(buffer, "this is server! welcome!\n");
                
                send(clifd, buffer, MAX_BUFFER, 0);
                printf("new connection client[%d] %s:%d\n", 
                        conn_amount, 
                        inet_ntoa(cli_addr.sin_addr), 
                        ntohs(cli_addr.sin_port));
                bzero(buffer, sizeof(buffer));

                ret = recv(clifd, buffer, MAX_BUFFER, 0);
                if (ret < 0) {
                    perror("recv error!");
                    close(serverfd);
                    return -1;
                }
                printf("recv: %s\n", buffer);

                if (clifd >  maxfd) {
                    maxfd = clifd;
                }
                else {
                    printf("max connections!!! quit!!\n");
                    break;
                }
            }
        }
    }
    for (int i = 0; i < MAX_LISTEN_NUM; ++i) {
        if (client_sockfd[i] != 0) {
            close(client_sockfd[i]);
        }
    }
    close(serverfd);
    return 0;
}
