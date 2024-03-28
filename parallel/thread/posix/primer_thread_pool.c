#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define LEFT  30000000
#define RIGHT 30000200
#define THRNUM 4 // 线程个数
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static int number = 0;
static void *thr_prime(void *p) {
    int i, j, mark;
    while(1){
        pthread_mutex_lock(&mut);
        while(number == 0){
            // printf("thread %d: number = %d, waiting...\n", (int)p, number);
            pthread_mutex_unlock(&mut);
            sched_yield();
            pthread_mutex_lock(&mut);
        }
        if(number == -1){
            pthread_mutex_unlock(&mut);  //临界区内出现跳转语句要释放锁！
            break;
        }
        // printf("thread %d: number = %d, waiting...\n", (int)p, number);
        i = number;
        number = 0;  //不要忘记任务标志清0-
        pthread_mutex_unlock(&mut);

        mark = 1;
        for(j = 2; j < i/2; j++) {
            if(i % j == 0) {
                mark = 0;
                break;
            }
        }
        if(mark)
            printf("[%d]%d is a primer.\n", (int)p, i);
    }
	pthread_exit(NULL);
}

int main(void) {
    //线程池构造
	int i, err;
    pthread_t tid[THRNUM];
    for(i = 0; i <= THRNUM; i++) {
        // err = pthread_create(tid+(i-LEFT), NULL, thr_prime, &i);  
        err = pthread_create(tid+(i), NULL, thr_prime, (void *)i);  
        if(err) {  //这里只是一个示例，实际上还要判断比如创建到第n个时，创建失败，如何回收前面创建的线程
            fprintf(stderr, "pthread_create(): %s\n", strerror(err));
            exit(1);
        }
    }
    
    //任务发配   //上下游都用了忙等法
    for(i = LEFT; i <= RIGHT; i++){
        pthread_mutex_lock(&mut);
        while(number != 0){  //如果还有任务被残留在变量里
            pthread_mutex_unlock(&mut);   //释放锁，让调度器转换到另一个线程去执行
            sched_yield();     //让出cpu， 可以不严格的理解为非常短暂的一个sleep()
            pthread_mutex_lock(&mut);
        }
        number = i;
        // printf("main(): queueing task %d, num=%d\n", i, number);
        // printf("%d\n",number);
        pthread_mutex_unlock(&mut);
    }
    //任务结束
    pthread_mutex_lock(&mut);
    while(number > 0){   //防止上面循环跳出后的最后一个数值还没来得及计算便被-1冲掉
        pthread_mutex_unlock(&mut);
        sched_yield();
        pthread_mutex_lock(&mut);
    }
    number = -1;
    pthread_mutex_unlock(&mut);

    for (i = 0; i <= THRNUM; i++)    //注意边界条件 
        pthread_join(tid[i], NULL);

    pthread_mutex_destroy(&mut);
    exit(0);
}


