/******************************************************************************
 *	File Name: ip_str_and_int.c
 *	Author: 公众号: Linux云计算网络
 *	Created Time: Sun 09 Dec 2018 11:47:28 AM CST
 *****************************************************************************/

//输入点分十进制的IP，将其转换成整型后，再反向转换进行验证
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>

unsigned int IPtoInt(char *str_ip)
{
    assert(str_ip);
    in_addr addr;
    unsigned int int_ip;
    if(inet_pton(AF_INET,str_ip,&addr)) {
        int_ip = ntohl(addr.s_addr);
    }
    return int_ip;
}

char *IpToDot(unsigned int nIp)
{
    in_addr addr;
    addr.s_addr = htonl(nIp); 
    char strip[INET_ADDRSTRLEN];
    const char *ptr = inet_ntop(AF_INET,&addr,strip,sizeof(strip));
    if(NULL != ptr){
        return strip;
    }
    return NULL;
}
