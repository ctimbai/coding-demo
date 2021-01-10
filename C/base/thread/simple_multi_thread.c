#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/* 子线程1入口函数 */
void *thread_routine1(void *arg)
{
    fprintf(stdout, "thread1: hello world!\n");
    sleep(1);

    /* 子线程在此退出 */
    return NULL;
}

/* 子线程2入口函数 */
void *thread_routine2(void *arg)
{
    fprintf(stdout, "thread2: I'm running...\n");
    pthread_t main_thread_id = *(pthread_t *)arg;

    /* 分离自我，不能再被连接 */
    pthread_detach(pthread_self());

    /* 判断主线程ID与子线程2 ID是否相等 */
    if (!pthread_equal(main_thread_id, pthread_self())) {
        fprintf(stdout, "thread2: main thread id is not equal ethread2!\n");
    }

    /* 等待主线程终止 */
    pthread_join(main_thread_id, NULL);
    fprintf(stdout, "thread2: main thread exit!\n");

    fprintf(stdout, "thread2: exit!\n");
    fprintf(stdout, "thread2: process exit!\n");

    /* 子线程2在此终止，进程退出 */
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    /* 创建子线程1 */
    pthread_t tid1;
    if (pthread_create(&tid1, NULL, thread_routine1, NULL) != 0) {
        fprintf(stderr, "create thread fail.\n");
        exit(-1);
    }

    /* 等待子线程1终止 */
    pthread_join(tid1, NULL);
    fprintf(stdout, "main thread: thread1 terminated!\n");

    /* 创建子线程 2 */
    pthread_t tid2;
    if (pthread_create(&tid2, NULL, thread_routine2, (void*)pthread_self()) != 0) {
        fprintf(stderr, "create thread fail.\n");
        exit(-1);
    }

    fprintf(stdout, "main thread: sleeping...\n");
    sleep(3);

    /* 主线程使用 pthread_exit 函数终止，进程继续存在 */
    fprintf(stdout, "main thread: exit!\n");
    pthread_exit(NULL);

    fprintf(stdout, "main thread: never reach here!\n");
    return 0;
}