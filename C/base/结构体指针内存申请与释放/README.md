```c
#include <stdio.h>

int main()
{
    char **a;
    int i;
    a = (char **)malloc(sizeof(char *) * m);//分配指针数组
    for(i=0; i <m; i++) {
        a[i] = (char *)malloc(sizeof(char) * n);//分配每个指针所指向的数组
    }
    printf("%d\n", sizeof(a));//4，指针
    printf("%d\n", sizeof(a[0]));//4，指针
    for(i=0; i<m; i++) {
        free(a[i]);
    }
    free(a);
}
```
