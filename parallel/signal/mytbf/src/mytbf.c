#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "mytbf.h"
#include <signal.h>
#include <unistd.h>
/**
 *1、数据结构如下
 *      用一个数组来保存每个mytbf_st类型的结构体，该数组的大小为MYTBF_MAX。（pos自述下标利用了数组快速查找的功能，实际上也可用链表实现该库，但是效率不高）
 *      每个mytbf_st结构体中包含三个成员：cps、burst、token，分别表示每秒(时间控制用alarm)可获取的令牌数、令牌的总数量最大值和当前令牌数量。
 *2、由1可知，数据结构的创建可以分为两个部分，mytbf_st的创建和存储这些结构体指针的数组；我这里简称小内存管理和大内存管理。
 *      小内存管理：mytbf_init()函数，创建mytbf_st结构体，返回mytbf_st指针。mytbf_destroy()函数，释放mytbf_st结构体，释放mytbf_t指针。
 *      大内存管理：moudle_unload()钩子函数用于释放job数组空间；get_free_pos()函数用于获取job中一个空闲位置，返回空闲位置的下标。
 * 3、alarm和signal组合来进行时间控制。
 *      一旦进行module_load()，则会注册一个SIGALRM信号的处理函数，该函数为alrm_handler()，该函数的主要功能是：每1秒，将所有令牌数量加cps，然后将令牌数量限制在burst以内。
 *      同时module_unload将被放置于钩子上
 */

struct mytbf_st{
    int cps;
    int burst;
    int token;
    int pos;  //自述下标
};

static struct mytbf_st* job[MYTBF_MAX];   //全局+静态变量自动初始化为全NULL
static int inited = 0;

static int get_free_pos(void){
    for (int i = 0; i < MYTBF_MAX; i++) // 遍历 job 数组
    {
        if(job[i] == NULL) // 找到第一个空闲位置
            return i;
    }
    return -1; // 如果没有空闲位置，返回 -1
}

static void alrm_handler(int s){
    alarm(1);
    for (int i = 0; i < MYTBF_MAX; i++)
    {
        if(job[i] != NULL){
            job[i]->token += job[i]->cps;
            if(job[i]->token > job[i]->burst)
                job[i]->token = job[i]->burst;
        }
    }
} //如果for循环太长，消耗的时间大于1s，会不会出现重入？或者无限卡住

typedef void (*sighandler_t)(int);
static sighandler_t alrm_handler_save;
static void module_unload(void){
    signal(SIGALRM, alrm_handler_save);
    alarm(0);
    for (int i = 0; i < MYTBF_MAX; i++)
    {
            free(job[i]);  //free对NULL的释放不会出错
            job[i] = NULL;
    }
    
}
static void module_load(void){
    alrm_handler_save = signal(SIGALRM, alrm_handler);
    alarm(1);
    atexit(module_unload);
}


mytbf_t * mytbf_init(int cps, int burst){
    struct mytbf_st *me;

    if(!inited){
        module_load();
        inited = 1;
    }

    int pos = get_free_pos();
    if(pos < 0){
        return NULL;
    }

    me = malloc(sizeof(*me));
    if(me == NULL){
        return NULL;
    }
    me->token = 0;
    me->cps = cps;
    me->burst = burst;
    me->pos = pos;

    job[pos] = me;

    return me;
}
static int min(int a, int b){
    return a < b ? a : b;
}
int mytbf_fetchtocken(mytbf_t * tbf, int size){
    struct mytbf_st *me = tbf;

    if(size <= 0)
        return -EINVAL;
    while(me->token <= 0)
        pause();
    int n = min(me->token, size);
    me->token -= n;
    return n;
}

int mytbf_returntoken(mytbf_t * tbf, int size){
    struct mytbf_st *me = tbf;
    if(size <= 0)
        return -EINVAL;
    me->token += size;
    if(me->token > me->burst)
        me->token = me->burst;
    return size;   //就算满了也根用户说全部受到
}

int mytbf_destroy(mytbf_t * tbf){
    struct mytbf_st *me = tbf;   //强转

    job[me->pos] = NULL;
    free(tbf);
    return 0;

}