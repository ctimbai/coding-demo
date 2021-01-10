#include <stdio.h>
#include <string.h>
#include <pthread.h>

struct para_type {
    int a;
    char b[20];
};

void* say_hello(void *args)
{
    struct para_type para = *(struct para_type *)args;
    printf("hello from thread %d, string is: %s\n", para.a, para.b);
    pthread_exit((void*) 2);
}

int main(int argc, char const *argv[])
{
    pthread_t tid;
    int ret;
    struct para_type para;
    para.a = 10;
    char s[20] = "hello world!";
    strncpy(para.b, s, sizeof(s));
    ret = pthread_create(&tid, NULL, say_hello, &para);
    if (ret) {
        printf("pthread create error: error num=%d\n", ret);
        return ret;
    }

    void *retval;
    ret = pthread_join(tid, &retval);
    if (ret) {
        printf("pthread_join error, error is: %d\n", ret);
        return ret;
    }

    printf("retval=%ld\n", (long)retval);

    return 0;
}