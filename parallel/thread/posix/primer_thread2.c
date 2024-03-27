#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define LEFT  30000000
#define RIGHT 30000200
#define THRNUM (RIGHT-LEFT+1)
// 一个进程中能创建几个线程？取决于ulimit中的栈大小限制。每个线程会占用独立的2栈资源。而32位机器的进程空间有限
//每个线程中，栈是独立的，代码段是共用的
struct thr_arg_st {
    int n;
};

static void *thr_prime(void *p) {
    int i, j, mark;
    // 先将void*强转为struct thr_arg_st *
    i = ((struct thr_arg_st *)p) -> n;
    mark = 1;
    for(j = 2; j < i/2; j++) {
        if(i % j == 0) {
            mark = 0;
            break;
        }
    }
    if(mark)
        printf("%d is a primer.\n", i);
    // 将p作为返回值
	pthread_exit(p);
}

int main(void) {
	int i, err;
    pthread_t tid[THRNUM];
    struct thr_arg_st *p;
    void * ret;
    for(i = LEFT; i <= RIGHT; i++) {
        // 动态分配内存
        p = malloc(sizeof(*p));
        if(p == NULL) {
            perror("malloc()");
            exit(1);
        }
        p -> n = i; 
        err = pthread_create(tid+(i-LEFT), NULL, thr_prime, p);
        if(err) {
            fprintf(stderr, "pthread_create(): %s\n", strerror(err));
            exit(1);
        }
    }

    for (i = LEFT; i <= RIGHT; i++) {
        // 用ret来接收（一级指针的地址）
        pthread_join(tid[i-LEFT], &ret);
        // 释放动态分配的内存
        free(ret);
    }
    exit(0);
}

