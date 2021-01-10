/******************************************************************************
 *	File Name: get_cpu_mem.c
 *	Author: 公众号: Linux云计算网络
 *	Created Time: 2018年11月12日 星期一 12时33分04秒
 *****************************************************************************/

#include <stdio.h>
#include <unistd.h>  
#include <errno.h>
#include <string.h>

void sysinfo_print()
{
    int cpu_num, cpu_online_num;
    int mem_size, mem_free_size;
 
    cpu_num = sysconf(_SC_NPROCESSORS_CONF);
    if (cpu_num != -1) {
        printf("The number of processors: %d\n", cpu_num);
    } else {
        printf("Failed to get number of processors: %s\n", strerror(errno));
    }
 
    cpu_online_num = sysconf(_SC_NPROCESSORS_ONLN);
    if (cpu_online_num) {
        printf("The number of online processors: %d\n", cpu_num);
    } else {
        printf("Failed to get number of online processors: %s\n",
               strerror(errno));
    }
 
    // 注意: OSX 不支持下面两个宏.
    mem_size = sysconf(_SC_PHYS_PAGES);
    if (mem_size) {
        printf("The memory size: %dK\n", mem_size * 4);
    } else {
        printf("Failed to get memory size: %s\n", strerror(errno));
    }
 
    mem_free_size = sysconf(_SC_AVPHYS_PAGES);
    if (mem_free_size) {
        printf("The free memory size: %dK\n", mem_free_size * 4);
    } else {
        printf("Failed to get free memory size: %s\n", strerror(errno));
    }
 
}
 
int main(int argc, char *argv[])
{
    sysinfo_print();
 
    return 0;
}
