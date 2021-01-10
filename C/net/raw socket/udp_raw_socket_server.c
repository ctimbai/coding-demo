#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/if_ether.h>
#include <stdlib.h>
#include <arpa/inet.h>

int main()
{
    printf("main is running\n");

    int iSock, nRead, iProtocol;
    char buffer[4096] = {0};
    char  *ethhead, *iphead, *tcphead, *udphead, *icmphead, *p;

    if((iSock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_IP))) < 0)
    {
        printf("create iSocket error, check root\n");  // 需要root权限， 最后运行的时候， 可以用sudo ./server
        return 1;
    }

    while(1)
    {
        nRead = recvfrom(iSock, buffer, 2048, 0, NULL, NULL);
        /*
            以太网帧头 14
            ip头       20
            udp头      8
            总共42字节(最少)
        */
        if(nRead < 42)
        {
            printf("packet error\n");
            continue;
        }

        int n = 0XFF;
        char szVisBuf[1024] = {0};
        for(unsigned int i = 0; i < nRead; ++i)
        {
            char szTmp[3] = {0};
            sprintf(szTmp, "%02x", buffer[i]&n);
            strcat(szVisBuf, szTmp);
        }


        ethhead = buffer;
        p = ethhead;

        iphead = ethhead + 14;
        p = iphead + 12;

        char szIps[128] = {0};
        snprintf(szIps, sizeof(szIps), "IP: %d.%d.%d.%d => %d.%d.%d.%d",
            p[0]&n, p[1]&n, p[2]&n, p[3]&n,
            p[4]&n, p[5]&n, p[6]&n, p[7]&n);
        iProtocol = (iphead + 9)[0];
        p = iphead + 20;


        unsigned int iDstPort = (p[2]<<8)&0xff00 | p[3]&n;
        switch(iProtocol)
        {
            case IPPROTO_UDP :
                if(iDstPort == 8888)
                {
                    printf("source port: %u,",(p[0]<<8)&0xff00 |  p[1]&n);
                    printf("dest port: %u\n", iDstPort);

                    printf("%s\n", szIps);
                    printf("%s\n", szVisBuf);
                    printf("nRead is %d\n", nRead);

                }
                break;
            case IPPROTO_RAW :
                printf("raw\n");
                break;
            default:
                break;
        }
    }
}
