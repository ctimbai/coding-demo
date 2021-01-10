#include <stdio.h>
#include <unistd.h>

int main()
{
	int cpu_num;
    int mem_num;

	cpu_num = sysconf(_SC_NPROCESSORS_CONF);
	printf("_SC_NPROCESSORS_CONF=%d\n", cpu_num);

	cpu_num = sysconf(_SC_NPROCESSORS_CONF);
	printf("_SC_NPROCESSORS_ONLN=%d\n", cpu_num);

    mem_num = sysconf(_SC_PHYS_PAGES);
    printf("_SC_PHYS_PAGES=%d\n", mem_num);
	return 0;
}
