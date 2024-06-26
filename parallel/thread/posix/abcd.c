#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#define THRNUM 4

static pthread_mutex_t mut[THRNUM];
static int next(int n){
    return (n+1) == THRNUM ? 0 : n+1;
}

static void *thr_func(void *p){
    int n = (int)p;
    int ch = 'a' + n;

    while(1){
        pthread_mutex_lock(&mut[n]);
        write(1, &ch, 1);
        pthread_mutex_unlock(&mut[next(n)]);
    }
    pthread_exit(NULL);
}

int main(){
    pthread_t tid[THRNUM];
    int err;
    for(int i = 0; i < THRNUM; i++){
        pthread_mutex_init(&mut[i], NULL);

        pthread_mutex_lock(&mut[i]);   //保证先创建完所有子线程再开始打印？
        err = pthread_create(&tid[i], NULL, thr_func, (void*)i);
        if(err){
            fprintf(stderr, "pthread_create(): %s\n", strerror(err));
            exit(1);
        }
    }
    pthread_mutex_unlock(&mut[0]);
    alarm(5);
    for(int i = 0; i < THRNUM; i++){
        pthread_join(tid[i], NULL);
    }

    exit(0);
}