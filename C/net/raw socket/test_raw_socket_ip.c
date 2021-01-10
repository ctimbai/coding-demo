#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <linux/tcp.h>
#include <linux/upd.h>

#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <linux/sockios.h>

unsigned
csum_tcpudp_nofold(unsigned saddr, unsigned daddr,
            unsigned len, unsigned proto, unsigned sum)
{
    unsigned long long s = (unsigned)sum;
    s += (unsigned)saddr;
    s += (unsigned)daddr;
    s += (proto + len) << 8;
    s += (s >> 32);
    return (unsigned)s;
}

unsigned short
check_sum(unsigned short *addr, int len, unsigned sum)
{
    int nleft = len;
    unsigned short *w = addr;
    unsigned short ret = 0;
    while (nleft > 1) {
        sum += *w++;
        nleft -= 2;
    }
    if (nleft == 1) {
        *(unsigned char *)(&ret) = *(unsigned char *)w;
        sum += ret;
    }

    sum = (sum>>16) + (sum&0xffff);
    sum += (sum>>16);
    ret = ~sum;
    return ret;
}

/*
 * 构造 IP 报文，最后 sendto 报文
 */
void
packet_build(int fd, struct sockaddr_in *target, unsigned short srcport)
{
    char buf[256] = {0};
    struct ip *p;
    struct tcphdr *tcp;
    int ip_len;
    int op_len = 12;

    /* 不考虑 data 部分 */
    ip_len = sizeof(struct ip) + sizeof(struct tcphdr) + op_len;

    /* 填充 IP 首部 */
    ip = (struct ip*)buf;
    ip->ip_v = IPVERSION;
    ip->ip_hl = sizeof(struct ip) >> 2;
    ip->ip_tos = 0;
    ip->ip_len = htons(ip_len);
    ip->ip_id = 0;
    ip->ip_off = 0;
    ip->ip_ttl = MAXTTL;
    ip->ip_p = IPPROTO_TCP;
    ip->ip_sum = 0;
    ip->ip_dst = target->sin_addr;

    /* 填充 TCP 首部 */
    tcp = (struct tcphdr*)(buf + sizeof(struct ip));
    tcp->source = htons(srcport);
    tcp->dest = target->sin_port;
    srand(time(NULL));
    tcp->doff = (sizeof(struct tcphdr) + op_len) >> 2; // tcphdr+option
    tcp->syn = 1;
    tcp->check = 0;
    tcp->window = ntohs(14600);

    // 填充选项字段
    int i = ip_len - op_len;
    // mss = 1460
    buf[i++] = 0x02;
    buf[i++] = 0x04;
    buf[i++] = 0x05;
    buf[i++] = 0xb4;

    // sack
    buf[i++] = 0x01;
    buf[i++] = 0x01;
    buf[i++] = 0x04;
    buf[i++] = 0x02;

    // wsscale = 7
    buf[i++] = 0x01;
    buf[i++] = 0x03;
    buf[i++] = 0x03;
    buf[i++] = 0x07;

    int t = 1;
    while (1) {
        if (t == 0) break;
        t --;
        // 源地址随机
        tcp->seq = random();
        // 源地址自定义 192.168.204.136
        ip->ip_src.s_addr = 0x88cca8c0;
        unsigned sum = csum_tcpudp_nofold(ip->ip_src.s_addr, ip->ip_dst.s_addr,
            sizeof(struct tcphdr) + op_len, IPPROTO_TCP, 0);
        tcp->check = check_sum((unsigned short*)tcp,
            sizeof(struct tcphdr) + op_len, sum);
        //ip->ip_sum = check_sum((unsigned short*)ip, sizeof(struct ip), 0);
        sendto(fd, buf, ip_len, 0, (struct sockaddr*)target,
            sizeof(struct sockaddr_in));
    }
}

int
main(int argc, char const *argv[])
{
    /* code */
    int fd;
    struct sockaddr_in target;
    struct hostent *host;
    const int on = 1;
    unsigned short srcport;

    if (argc != 4) {
        printf("Usage: %s dstip dstport srcport\n", argv[0]);
        exit(1);
    }

    bzero(&target, sizeof(struct sockaddr_in));
    target.sin_family = AF_INET;
    target.sin_port = htons(argv[2]);

    if (inet_aton(argv[1], &target.sin_addr) == 0) {
        host = gethostbyname(argv[1]);
        if (host == NULL) {
            printf("Target name error!\n", hstrerror(h_errno));
            exit(1);
        }
        target.sin_addr = *(struct in_addr *)(host->h_addr_list[0]);
    }

    /* 创建一个 TCP 的原始套接字 */
    if ((fd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) < 0) {
        perror("Create Error!\n");
        exit(1);
    }

    /* 开启 IP_HDRINCL 特性，手动构造 IP 报文 */
    if (setsockopt(fd, IPPROTO_TCP, IP_HDRINCL, &on, sizeof(on)) < 0) {
        perror("Set IP_HDRINCL Error!\n");
        exit(1);
    }

    setuid(getpid());
    srcport = atoi(argv[3]);
    packet_build(fd, &target, srcport);

    return 0;
}