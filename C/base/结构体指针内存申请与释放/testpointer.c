/******************************************************************************
 *	File Name: testpointer.c
 *	Author: 公众号: CloudDeveloper
 *	Created Time: Sun 02 Dec 2018 02:15:58 PM CST
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define LEN 20
#define NUM 2

struct st1 {
    void *p;
    char c;
};


struct st1 *t[NUM];

int get(int key, void **p)
{
    *p = t[key]->p;
    return strlen(t[key]->p);
}

void put(int key, char *value)
{
    memcpy(t[key]->p, value, strlen(value));
}
int main()
{
    int i;
    char *str1 = "hello";
    char *str2 = "world";
    t[0] = (struct st1 *)malloc(sizeof(struct st1 *) * NUM);
    for (i = 0; i < NUM; i ++) {
        t[i] = malloc(sizeof(struct st1));
        t[i]->p = malloc(LEN);
    }
    memcpy(t[0]->p, str1, strlen(str1));
    t[0]->c = 'a';
    memcpy(t[1]->p, str2, strlen(str2));
    t[1]->c = 'b';

    for (i = 0; i < NUM; i ++) {
        printf("line %d: %s, %c\n", i, t[i]->p, t[i]->c);
    }
    
    void *str3 = NULL;
    int len;
    len = get(0, &str3);
    printf("get str: %s, %d\n", str3, len);

    char *str4 = "hhhhh";
    put(1, str4);
    printf("put str: %s, %d\n", t[1]->p, strlen(t[1]->p));

    put(1, str3);
    printf("put str: %s, %d\n", t[1]->p, strlen(t[1]->p));
    return 0;
}
