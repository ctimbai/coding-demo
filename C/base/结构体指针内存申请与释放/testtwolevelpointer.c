/******************************************************************************
 *	File Name: testtwolevelpointer.c
 *	Author: 公众号: CloudDeveloper
 *	Created Time: Fri 30 Nov 2018 09:22:32 AM CST
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct twopointer {
    void *pd;
}tp;

typedef struct onepointer {
    tp t;
    int i;
}op;

int getdata(op *p, void **d)
{
    int len;
    *d = p->t.pd;
    len = strlen(p->t.pd);
    return len;
}
int main()
{   
    op o1 = {
        .t = {
            .pd = "hello world"
        },
        .i = 4
    };

    void *p;
    int len;
    //void *tmp = malloc(20);
    //memset(tmp, 0, 20);
    void *tmp;
    len = getdata(&o1, &p);
    memcpy(tmp, p, len);
    printf("len=%d\n", len);
    printf("%s, p.len=%d\n", p, strlen(p));
    printf("tmp: %s, tmp.len=%d\n", tmp, strlen(tmp));
    return 0;
}
