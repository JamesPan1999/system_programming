#include "stdlib.h"
#include "stdio.h"
#include "pthread.h"

#include "mypipe.h"

struct mypipe_st{
    int head;
    int tail;
    char data[PIPESIZE];
    int dataSize;
    pthread_mutex_t mutex;      // 保证管道资源互斥
    pthread_cond_t cond;        // 用于实现通知法

};

mypipe_t *mypipe_init(void){
    struct mypipe_st *me = malloc(sizeof(struct mypipe_st));
    if(me == NULL)
        return NULL;

    me->head = 0;
    me->tail = 0;
    me->dataSize = 0;
    pthread_mutex_init(&me->mutex, NULL);
    pthread_cond_init(&me->cond, NULL);

    return me;
}

/**
 * static保证仅文件内可见，struct mypipe_st *me 而不用mypipe_t *p 主要是自己用的舒服，该接口不对外
 * unlocked 的命名表示其线程不安全，需要在上层加锁
 * @param me
 * @param buf 要把读到的数据放到的地方
 * @return
 */
static int mypipe_readbyte_unlocked(struct mypipe_st *me, char *datap){
    if(me->dataSize <= 0)
        return -1;
    *datap = me->data[me->head];
    me->head = (me->head + 1) % PIPESIZE;  // 循环队列，head++会加出边界
    me->dataSize--;
    return 0;
}

int mypipe_read(mypipe_t *p, char *buf, size_t count){
    struct mypipe_st *me = p;
    pthread_mutex_lock(&me->mutex);
    while(me->dataSize == 0){
        pthread_cond_wait(&me->cond, &me->mutex);
    }
    int i = 0;
    for(i = 0; i < count; i++){
        // for+if-break 实现了 count 和 me->dataSize 两者取其小
        if(mypipe_readbyte_unlocked(me, buf + 1) != 0)
            break;   // 注意有没有跳出临界区，否则会死锁
    }

    pthread_mutex_unlock(&me->mutex);
    return i;   // 读取的字节数
}

int mypipe_write(mypipe_t *p, const char *buf, size_t count){

}

int mypipe_destroy(mypipe_t *p){
    struct mypipe_st *me = p;
    pthread_mutex_destroy(&me->mutex);
    pthread_cond_destroy(&me->cond);
    free(me);
    return 0;
}
