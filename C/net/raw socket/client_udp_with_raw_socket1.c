#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>          // socket
#include <sys/socket.h>         // socket
#include <sys/ioctl.h>          // ioctl
#include <net/if.h>             // ifreq
#include <linux/if_packet.h>    // sockaddr_ll
#include <arpa/inet.h>          // inet_addr
#include <netinet/ip.h>
#include <linux/udp.h>

#define NUMPKTS 64
#define BUFLEN 100
#define COUNTS 10

#define PROTCP IPPROTO_TCP
#define PROUDP IPPROTO_UDP

#define SERVER_PORT 8888
#define SERVER_IP "127.0.0.1"
#define CLIENT_PORT 6666
#define CLIENT_IP "127.0.0.2"

/* 伪头部 */
struct udp_psd_header {
    u_int32_t src;
    u_int32_t des;
    u_int8_t  mbz;
    u_int8_t ptcl;
    u_int16_t len;
};

/* 计算校验和 */
unsigned short
csum(unsigned short *buf, int nwords)
{
    unsigned long sum;
    for (sum = 0; nwords > 0; nwords--) {
        sum += *buf++;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

int
make_raw_socket(int protocol)
{
    int fd;
    int val = 1;

    fd = socket(AF_INET, SOCK_RAW, protocol);
    if (fd < 0) {
        perror("socket() error!");
        exit(EXIT_FAILURE);
    }

    /* 开启 IP_HDRINCL 特性，手动构造 IP 报文 */
    if (setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &val, sizeof(val))) {
        perror("setsockopt() for IP_HDRINCL error");
        exit(EXIT_FAILURE);
    }
#ifndef NODEBUG
    else {
        printf("setsockopt() for IP_HDRINCL is OK.\n");
    }
#endif
    return fd;
}

/*
 * 打印数据包的信息
 */
void
print_packet_info(struct ip *ip, struct udphdr *udp)
{
    printf("================= IP header ===============\n");
    printf("ip version: %d\n", ip->ip_v);
    printf("ip header len: %d\n", ip->ip_hl);
    printf("ip total len: %d\n", ntohs(ip->ip_len));
    printf("src ip: %s\n", (char *)inet_ntoa(ip->ip_src));
    printf("dst ip: %s\n", (char *)inet_ntoa(ip->ip_dst));

    printf("================= UDP header ===============\n");
    printf("udp header len: %d\n", ntohs(udp->len));
    printf("udp header len: %d\n", udp->len);
    printf("udp checksum: 0x%04x\n", udp->check);
    printf("udp src port: %d\n", ntohs(udp->source));
    printf("udp src port: %d\n", ntohs(udp->dest));
}

/*
 * 构造报文
 */
int
setup_packet(int fd, char *buffer, struct sockaddr_in *saddr,
    struct sockaddr_in *daddr)
{
    struct ip *ip;
    //struct tcphdr *tcp;
    struct udphdr *udp;
    int ip_len, udplen;
    int udphostlen;
    unsigned char str[] = "";

    char idinfo[] = "1234";
    void *p = NULL;

    /* 填充 IP 首部 */
    ip = (struct ip *)buffer;
    ip->ip_v = IPVERSION;
    ip->ip_hl = sizeof(struct ip) >> 2;
    //ip->ip_hl = 6;
    ip->ip_tos = 0;
    ip_len = sizeof(struct ip) + sizeof(struct udphdr) + sizeof(str);
    //ip_len = (ip->ip_hl << 2) + sizeof(struct udphdr) + sizeof(str);
    ip->ip_len = htons(ip_len);
    ip->ip_id = htons(4);
    ip->ip_off = 0;
    ip->ip_ttl = MAXTTL;
    ip->ip_p = IPPROTO_UDP;
    ip->ip_sum = 0;
    ip->ip_src = saddr->sin_addr;
    ip->ip_dst = daddr->sin_addr;

    // p = (void *)(buffer + sizeof(struct ip));
    // memcpy(p, idinfo, sizeof(idinfo));

    // printf("option: %s\n", p);
    printf("ip id: %d\n", ip->ip_id);
    /* 填充 UDP 首部 */
    //udp = (struct udphdr *)(buffer + sizeof(struct ip));
    udp = (struct udphdr *)(buffer + (ip->ip_hl << 2));
    udp->source = saddr->sin_port;
    udp->dest = daddr->sin_port;
    udp->len = htons(sizeof(struct udphdr) + sizeof(str));
    udphostlen = ntohs(udp->len);
    /* 计算checksum 伪头部+UDP头部+数据部分 */
    udplen = sizeof(struct udp_psd_header) + sizeof(struct udphdr) + sizeof(str) + 1;
    char *cksum = malloc(udplen);
    memset(cksum, 0, udplen);
    struct udp_psd_header *uph = (struct udp_psd_header *)cksum;

    uph->src = saddr->sin_addr.s_addr;
    uph->des = daddr->sin_addr.s_addr;
    uph->mbz = 0;
    uph->ptcl = 17;
    uph->len = htons(sizeof(struct udphdr) + sizeof(str));
    memcpy(cksum + sizeof(struct udp_psd_header), udp, sizeof(struct udphdr));
    memcpy(cksum + sizeof(struct udp_psd_header) + sizeof(struct udphdr), str,
        sizeof(str));
    udp->check = csum((unsigned short *)cksum, (sizeof(struct udphdr) +
        sizeof(struct udp_psd_header) + sizeof(str) + 1) / 2);

    printf("client udp checksum: 0x%04x\n", udp->check);
    /* 将数据拷贝进缓冲区 */
    memcpy(buffer + sizeof(struct ip) + sizeof(struct udphdr), str, sizeof(str));
    //memcpy(buffer + (ip->ip_hl << 2) + sizeof(struct udphdr), str, sizeof(str));
    print_packet_info(ip, udp);


    return ip_len;
}

/* 发送数据包 */
int

send_packets(int fd, char *buffer, int len, struct sockaddr_in daddr)
{
    int n;
    n = sendto(fd, buffer, len, 0, (struct sockaddr *)&daddr, sizeof(daddr));
    if ( n < 0) {
        printf("sendto() error!, error: %s\n", strerror(errno));
        return 0;
    }
#ifndef NODEBUG
    else {
        printf("Count #%u - sendto() is ok.\n", n);
        return 1;
    }
#endif
}

/* 接收数据包 */
int
recv_packets(int fd, char *buffer, int len)
{
    int n;
    n = recvfrom(fd, buffer, len, 0, NULL, NULL);
    if (n < 0) {
        printf("Failed to recv packets!\n");
        exit(EXIT_FAILURE);
    }
    else {
        printf("recv str is: %s\n", buffer);
    }
}

int
main(int argc, char const *argv[])
{
    int fd;
    int protocol;
    int val = 1;
    struct sockaddr_in saddr, daddr;

    if (argc < 5) {
        printf("- Invalid parameters!!!\n");
        printf("Usage: %s srcip srcport dstip dstport\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    protocol = PROUDP;
    fd = make_raw_socket(protocol);
    if (fd >= 0) {
        printf("socket() - Using SOCK_RAW socket and UDP protocol is OK.\n");
    }

    bzero(&saddr, sizeof(struct sockaddr_in));
    bzero(&daddr, sizeof(struct sockaddr_in));
    saddr.sin_family = AF_INET;
    daddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = inet_addr(argv[1]);
    daddr.sin_addr.s_addr = inet_addr(argv[3]);
    saddr.sin_port = htons(atoi(argv[2]));
    daddr.sin_port = htons(atoi(argv[4]));

    char buf[BUFLEN];
    memset(buf, 0, BUFLEN);
    int tolen = setup_packet(fd, buf, &saddr, &daddr);
    printf("packet len: %d\n", tolen);
    printf("buf: %s\n", buf);
    // 如果不是 root 用户，需要获取权限
    setuid(getpid());

    /* Send loop, send for every 2 second for 2000000 count */
    printf("Trying...\n");
    printf("Using raw socket and UDP protocol\n");
    printf("Using Source IP: %s port: %u, Target IP: %s port: %u.\n",
        argv[1], atoi(argv[2]), argv[3], atoi(argv[4]));
    printf("Ip length: %d\n", tolen);

    //int i = 0;
    // while (i < COUNTS) {
    //     if (send_packets(fd, buf, tolen, daddr) == 0) {
    //         perror("Error sending packet!");
    //         exit(EXIT_FAILURE);
    //     }
    //     else {
    //         printf("Packet sent successfully\n");
    //     }
    //     i ++;
    // }

    char recvbuf[BUFLEN];
    memset(recvbuf, 0, BUFLEN);

    if (send_packets(fd, buf, tolen, daddr) == 0) {
        perror("Error sending packet!");
        exit(EXIT_FAILURE);
    }
    else {
        printf("Packet sent successfully\n");
        //recv_packets(fd, recvbuf, BUFLEN);
    }

    close(fd);
    return 0;
}