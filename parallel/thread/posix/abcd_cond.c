#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#define THRNUM 4

static pthread_mutex_t mut;
static pthread_cond_t cond;
static int number = 0;

static int next(int n){
    return (n+1) == THRNUM ? 0 : n+1;
}

static void *thr_func(void *p){
    int n = (int)p;
    int ch = 'a' + n;
    while(1){
        pthread_mutex_lock(&mut);
        while(n != number){
            pthread_cond_wait(&cond, &mut);
        }
        write(1, &ch, 1);
        number = next(number);
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mut);
    }


    pthread_exit(NULL);
}

int main(){
    pthread_t tid[THRNUM];
    int err;
    for(int i = 0; i < THRNUM; i++){

        // pthread_mutex_lock(&mut);
        err = pthread_create(&tid[i], NULL, thr_func, (void*)i);   //上一行的锁对线程函数无效
        if(err){
            fprintf(stderr, "pthread_create(): %s\n", strerror(err));
            exit(1);
        }
    }
    // pthread_mutex_unlock(&mut);

    alarm(5);   //内存泄漏啦
    for(int i = 0; i < THRNUM; i++){
        pthread_join(tid[i], NULL);
    }

    pthread_mutex_destroy(&mut);
    pthread_cond_destroy(&cond);
    exit(0);
}