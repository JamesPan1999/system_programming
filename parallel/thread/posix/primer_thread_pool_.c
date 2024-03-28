#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define LEFT  30000000
#define RIGHT 30000200
#define THRNUM 4 // 假设线程池中有4个线程

// 临界区资源
static int num = 0;
// 互斥锁
static pthread_mutex_t mut_num = PTHREAD_MUTEX_INITIALIZER;

static void *thr_prime(void *p) {
    int i, j, mark;
    // 死循环：领取任务
    while(1) {
        pthread_mutex_lock(&mut_num);
        // 循环检测是否有任务
        while(num == 0) { // 当前没有任务
            pthread_mutex_unlock(&mut_num);
            sched_yield();
            pthread_mutex_lock(&mut_num);
        }
        // 拿到任务，判断是否为-1，即任务已经结束
        if(num == -1) {
            // 要释放锁，防止其他线程死锁
            pthread_mutex_unlock(&mut_num);
            break;
        }
        // 拿到任务
        i = num;
        // 将num设置为0，即没有任务
        num = 0;
        pthread_mutex_unlock(&mut_num);
        //---------线程做任务：检测是否为质数-------
        mark = 1;
        for(j = 2; j < i/2; j++) {
            if(i % j == 0) {
                mark = 0;
                break;
            }
        }
        if(mark)
            printf("[thread-%d]%d is a primer.\n", (int)p, i);
    }
    pthread_exit(NULL);
}

int main(void) {
	int i, err;
    pthread_t tid[THRNUM];
    // 启动线程池
    for(i = 0; i < THRNUM; i++) {
        err = pthread_create(tid+i, NULL, thr_prime, (void *)i);
        if(err) {
            fprintf(stderr, "pthread_create(): %s\n", strerror(err));
            exit(1);
        }
    }
	
    // 主线程下发任务
    for(i = LEFT; i <= RIGHT; i++) {
        pthread_mutex_lock(&mut_num);
        // 循环检测任务是否被线程领走
        // printf("main(): queueing task %d, num=%d\n", i, num);
        while(num != 0) { // 没有领走任务
            pthread_mutex_unlock(&mut_num);
            // 让出CPU
            sched_yield();
            pthread_mutex_lock(&mut_num);
        }
        // 设置num，即下发任务
        num = i;
        pthread_mutex_unlock(&mut_num);
    } // 下发任务完毕
    
    // 设置num为-1，代表任务全部结束
    pthread_mutex_lock(&mut_num);
    // 循环检测最后一个任务是否被完成
    while(num != 0) { // 没有完成
        pthread_mutex_unlock(&mut_num);
        sched_yield();
        pthread_mutex_lock(&mut_num);
    }
    // 已完成，则设置为-1
    num = -1;
    pthread_mutex_unlock(&mut_num);
	// 收尸
    for (i = 0; i < THRNUM; i++) {
        pthread_join(tid[i], NULL);
    }
	// 销毁互斥锁
    pthread_mutex_destroy(&mut_num);
    exit(0);
}
