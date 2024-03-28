#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define PATH "/tmp/out"
#define BUF_SIZE 1024
#define THENUM 10

static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;  //声明时就初始化（相当于调用了pthread_mutex_init

static void *thr_add(void * p){
    FILE *fp;
    char linebuf[BUF_SIZE];
    fp = fopen(PATH, "r+");
    if(fp == NULL){
        perror("fopen");
        exit(1);   //如果有其他线程的fopen打开失败，将会造成内存泄漏
    }

    pthread_mutex_lock(&mut);
    fgets(linebuf, BUF_SIZE, fp);   //注意每个线程内打开的fp相互独立，所以只要注意这个线程自身的fp就可以
    fseek(fp, 0, SEEK_SET);
    
    sleep(1);  //测试
    fprintf(fp, "%d\n", atoi(linebuf) + 1);
    
    fclose(fp);   //此时才刷新缓冲  注意缓冲区也要保证互斥
    pthread_mutex_unlock(&mut);

    pthread_exit(NULL);

}
int main()
{
	// pthread_mutex_t mutex;
	// pthread_mutex_init(&mutex, NULL);
	// pthread_mutex_lock(&mutex);
	// pthread_mutex_unlock(&mutex);
	// pthread_mutex_destroy(&mutex);
    pthread_t tid[THENUM];
    int err;
    for(int i = 0; i < THENUM; i++){
        err = pthread_create(&tid[i], NULL, thr_add, NULL);  //创建成功后返回err而不是thr_add结束后返回err
        if(err){
            fprintf(stderr, "pthread_create():%s\n", strerror(err));
            exit(1);
        }
    }
    
    for(int i = 0; i < THENUM; i++){
        pthread_join(tid[i], NULL);
    }
    pthread_mutex_destroy(&mut);

	exit(0);
}