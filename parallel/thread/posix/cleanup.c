#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

static void cleanup(void * p){
    puts(p);
}
static void *func(void *arg){
    puts("thread is working");
    pthread_cleanup_push(cleanup, "cleanup:1");  //注意这是宏，如果不写对应的pop将会在语法上缺括号，报错为主函数最后一个括号缺失
    pthread_cleanup_push(cleanup, "cleanup:2");
    pthread_cleanup_push(cleanup, "cleanup:3");
    puts("push over!");
    pthread_cleanup_pop(1);
    pthread_cleanup_pop(0);
    pthread_cleanup_pop(1);
    

    pthread_exit(NULL);    //很有意思的是，此函数中会调用没有进行pop的钩子函数。所以如果把pthread_cleanup_pup放在此句话后面，钩子函数也可以执行
    // pthread_cleanup_pop(1);
    // pthread_cleanup_pop(0);
    // pthread_cleanup_pop(1);

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