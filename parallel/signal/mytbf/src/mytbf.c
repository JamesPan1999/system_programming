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
 *      同时module_unload将被放置于钩子上。
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
static sighandler_t alrm_handler_save;   //有没有办法把这个丑陋的变量和module_load()和moudle_unload()放到一个结构体里面？
/**
 * 模块卸载函数
 * 该函数用于在模块卸载时进行清理工作。它将重置SIGALRM信号的处理函数，并取消任何设置的闹钟。
 * 然后，它会释放作业数组中所有分配的内存，并将数组中的每个元素设置为NULL。
 * 这里涉及到了编程的宏观思维：任何一个模块用完后，模块必须还原进入模块之前的现场。
 * 特别注意signal不能用SIG_DFT,因为进入现场前，不一定是SIG_DFT
 */
static void module_unload(void){
    // 将SIGALRM信号的处理函数重置为之前的保存值，并取消当前设置的任何闹钟
    signal(SIGALRM, alrm_handler_save);
    alarm(0);
    
    // 遍历作业数组，释放已分配的内存，并将数组元素设置为NULL
    for (int i = 0; i < MYTBF_MAX; i++)
    {
            free(job[i]);  // 释放作业的内存，对NULL的释放不会出错
            job[i] = NULL; // 将数组元素设置为NULL
    }
    
}
/**
 * 功能：加载模块
 * 
 * 本函数主要完成以下功能：
 * 1. 设置SIGALRM信号的处理函数为alrm_handler，并保存原先的处理函数；
 * 2. 触发一个1秒后的SIGALRM信号；
 * 3. 注册模块卸载函数module_unload，在程序正常退出时执行。
 */
static void module_load(void){
    // 设置SIGALRM信号的处理函数，并保存原有的处理函数
    alrm_handler_save = signal(SIGALRM, alrm_handler);
    // 触发一个1秒后的SIGALRM信号
    alarm(1);
    // 注册模块卸载函数，在程序退出时自动执行
    atexit(module_unload);
}


/**
 * 初始化令牌桶过滤器
 * 
 * 本函数用于初始化一个令牌桶过滤器实例，令牌桶算法是一种流量控制算法，可用于平滑流量。
 * 将会开启alarm进行计时
 * @param cps 每秒新增的令牌数量，用于控制流量的速率。
 * @param burst 令牌桶的最大容量，即允许突发传输的最大令牌数量。
 * @return 返回初始化好的令牌桶过滤器实例指针，若初始化失败则返回NULL。
 */
mytbf_t * mytbf_init(int cps, int burst){
    struct mytbf_st *me;

    // 确保模块只被初始化一次  仿造cpp的构造函数功能
    if(!inited){
        module_load(); // 加载模块
        inited = 1; // 设置初始化标志
    }

    // 尝试获取一个空闲位置来放置新的令牌桶实例
    int pos = get_free_pos();
    if(pos < 0){
        // 无法找到空闲位置，初始化失败
        return NULL;
    }

    // 分配内存给新的令牌桶实例
    me = malloc(sizeof(*me));
    if(me == NULL){
        // 内存分配失败，初始化失败
        return NULL;
    }
    // 初始化令牌桶实例的字段
    me->token = 0; // 初始化令牌数量为0
    me->cps = cps; // 设置每秒令牌生成速率
    me->burst = burst; // 设置令牌桶最大容量
    me->pos = pos; // 记录该令牌在数组中的位置

    // 将新创建的令牌桶实例放置在相应的位置
    job[pos] = me;

    // 返回成功初始化的令牌桶实例指针
    return me;
}
static int min(int a, int b){
    return a < b ? a : b;
}
/**
 * 从令牌桶中获取指定数量的令牌。
 * 
 * @param tbf 指向令牌桶结构体的指针。
 * @param size 请求获取的令牌数量。
 * @return 实际获取的令牌数量。如果请求的大小size小于等于0，返回-EINVAL。
 */
int mytbf_fetchtocken(mytbf_t * tbf, int size){
    struct mytbf_st *me = tbf; // 将类型转换为内部结构体指针。

    if(size <= 0) // 如果请求的令牌数量小于等于0，返回错误。
        return -EINVAL;  //加-是个技巧

    // 当令牌数量不足时，等待。pause和alarm一起用来实现不消耗cpu的阻塞。
    while(me->token <= 0)
        pause();

    int n = min(me->token, size); // 获取尽量多但不超过请求数量的令牌。
    me->token -= n; // 更新令牌桶中的令牌数量。
    return n; // 返回获取到的令牌数量。
}

/**
 * 向令牌桶中添加令牌。
 * 
 * @param tbf 指向令牌桶结构体的指针。
 * @param size 要添加的令牌数量。
 * @return 添加成功的令牌数量。如果输入数量小于等于0，返回-EINVAL错误码。
 */
int mytbf_returntoken(mytbf_t * tbf, int size){
    struct mytbf_st *me = tbf;
    // 检查要添加的令牌数量是否合法
    if(size <= 0)
        return -EINVAL;
    me->token += size; // 添加令牌
    // 确保令牌数量不超过桶的容量
    if(me->token > me->burst)
        me->token = me->burst;
    return size;   // 返回添加的令牌数量
}

int mytbf_destroy(mytbf_t * tbf){
    struct mytbf_st *me = tbf;   //强转

    job[me->pos] = NULL;
    free(tbf);
    return 0;

}