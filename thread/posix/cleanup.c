#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

static void cleanup(void * p){
    puts(p);
}
static void *func(void *arg){
    puts("thread is working");
    pthread_cleanup_push(cleanup, "cleanup:1");
    pthread_cleanup_push(cleanup, "cleanup:2");
    pthread_cleanup_push(cleanup, "cleanup:3");
    puts("push over!");
    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
    

    pthread_exit(NULL);
}

int main(){
    puts("Begin");
    pthread_t tid;
    int ret;
    ret = pthread_create(&tid, NULL, (void *)func, NULL);
    if(ret != 0){
        fprintf(stderr, "Error: pthread_create failed%s\n", strerror(ret));
        exit(1);
    }
    pthread_join(tid, NULL);
    puts("end");
    exit(0);
}