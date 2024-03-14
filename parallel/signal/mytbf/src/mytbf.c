#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "mytbf.h"
#include <signal.h>
#include <unistd.h>

struct mytbf_st{
    int cps;
    int burst;
    int token;
    int pos;  //自述下标
};

static struct mytbf_st* job[MYTBF_MAX];   //全局+静态变量自动初始化为全NULL
static int inited = 0;
static int get_free_pos(void){
    for (int i = 0; i < MYTBF_MAX; i++)
    {
        if(job[i] == NULL)
            return i;
    }
    return -1;
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
}

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