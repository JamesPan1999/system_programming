#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
static void * thread_1(void * p){
    puts("thread_1 is running");
    pthread_exit(NULL);
}

int main(){
    puts("main Begin");
    pthread_t thread_1_id;
    int ret = pthread_create(&thread_1_id, NULL, thread_1, NULL);
    if(ret != 0){
        fprintf(stderr, "pthread_creat() error:%s\n", strerror(ret));
        exit(1);
    }

    pthread_join(thread_1_id, NULL);   //阻塞等待 类似于wait
    puts("main end");
    exit(0);
}