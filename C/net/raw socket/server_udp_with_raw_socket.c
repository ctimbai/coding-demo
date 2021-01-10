#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>          // socket
#include <sys/socket.h>         // socket
#include <sys/ioctl.h>          // ioctl
#include <net/if.h>             // ifreq
#include <linux/if_packet.h>    // sockaddr_ll
#include <arpa/inet.h>          // inet_addr
#include <netinet/ip.h>
#include <linux/udp.h>

int
main(int argc, char const *argv[])
{
    /* code */
    int sock, n;
    char buffer[2048];
    // struct ethhdr *eth;
    // struct iphdr *iph;
    struct sockaddr_in servaddr, cliaddr, saddr, daddr;
    struct ip *iphdr;
    struct udphdr *udphdr;

    if (argc < 5) {
        printf("- Invalid parameters!!!\n");
        printf ("Usage: %s srcip srcport dstip dstport\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // create upd raw socket
    memset(buffer, 0, 2048);
    if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) < 0) {
        perror("socket error!\n");
        exit(1);
    }

    // bind socket
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(atoi(argv[2]));

    bzero(&cliaddr, sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_addr.s_addr = inet_addr(argv[3]);
    cliaddr.sin_port = htons(atoi(argv[4]));

    if (bind(sock, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in client_addr;
    socklen_t len;

    while (1) {
        printf("=====================================\n");
        n = recvfrom(sock, buffer, 2048, 0, (struct sockaddr *)&client_addr, &len);
        if (n < 0) {
            printf("Failed to get packets!\n");
            exit(EXIT_FAILURE);
        }
        else {
            printf("%d bytes read\n", n);
            printf("receive str is: %s\n", buffer);
            // if (sendto(sock, buffer, n, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)) < 0) {
            //     printf("Failed to sendto packets!\n");
            //     exit(EXIT_FAILURE);
            // }
            // else {
            //     printf("sendto success!\n");
            // }
        }

        // iphdr
        iphdr = (struct ip *)buffer;

        bzero(&saddr, sizeof(saddr));
        bzero(&daddr, sizeof(daddr));
        saddr.sin_addr = iphdr->ip_src;

        daddr.sin_addr = iphdr->ip_dst;

        printf("================= IP header ===============\n");
        printf("ip id: %d\n", ntohs(iphdr->ip_id));
        printf("ip version: %d\n", iphdr->ip_v);
        printf("ip tos: %d\n", iphdr->ip_tos);
        printf("ip header len: %d\n", iphdr->ip_hl);
        printf("ip total len: %d\n", ntohs(iphdr->ip_len));
        printf("src ip: %s\n", (char *)inet_ntoa(saddr.sin_addr));
        printf("dst ip: %s\n", (char *)inet_ntoa(daddr.sin_addr));

        // udphdr
        udphdr = (struct udphdr*)(buffer + sizeof(struct ip));

        printf("================= UDP header ===============\n");
        printf("udp header len: %d\n", ntohs(udphdr->len));
        printf("udp checksum: 0x%04x\n", udphdr->check);
        printf("udp src port: %d\n", ntohs(udphdr->source));
        printf("udp src port: %d\n", ntohs(udphdr->dest));
    }

    return 0;
}