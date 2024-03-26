#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define LEFT  30000000
#define RIGHT 30000200
#define THRNUM (RIGHT-LEFT+1) // 线程个数

static void *thr_prime(void *p) {
    int i, j, mark;
    // i = *(int *)p;
    i = (int)p;   //利用强转 但是会worinig
    mark = 1;
    for(j = 2; j < i/2; j++) {
        if(i % j == 0) {
            mark = 0;
            break;
        }
    }
    if(mark)
        printf("%d is a primer.\n", i);
	pthread_exit(NULL);
}

int main(void) {
	int i, err;
    pthread_t tid[THRNUM];  //linux下tid是个整数，但其他环境下可能是结构体或者其他东西 
    for(i = LEFT; i <= RIGHT; i++) {
        // err = pthread_create(tid+(i-LEFT), NULL, thr_prime, &i);  
        err = pthread_create(tid+(i-LEFT), NULL, thr_prime, (void *)i);  
        //注意pthread_create第一个参数是地址，和pthread_join的第一个参数不同，因为create要向该地址空间内写东西，而join只用读
        //&i 将会造成竞争冲突   如果就强行把i强转成void*以值的形式传入函数后，再强转回来，也是一个解决方案，就是会worning
        if(err) {  //这里只是一个示例，实际上还要判断比如创建到第n个时，创建失败，如何回收前面创建的线程
            fprintf(stderr, "pthread_create(): %s\n", strerror(err));
            exit(1);
        }
    }

    for (i = LEFT; i <= RIGHT; i++)
        pthread_join(tid[i-LEFT], NULL);

    exit(0);
}


