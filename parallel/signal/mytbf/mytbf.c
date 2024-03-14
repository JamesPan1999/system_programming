#include <stdio.h>
#include <stdlib.h>

#include "mytbf.h"


struct mytbf_st{
    int cps;
    int burst;
    int token;
    int pos;  //自述下标
};

static struct mytbf_st* job[MYTBF_MAX];   //自动初始化为全NULL

static int getfree_pos(){
    for (int i = 0; i < MYTBF_MAX; i++)
    {
        if(job[i] == NULL)
            return i;
    }
    return -1;
}
mytbf_t * mytbf_init(int cps, int burst){
    struct mytbf_st *me;

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

int mytbf_fetchtocken(mytbf_t * tbf, int token){

}

int mytbf_returntoken(mytbf_t * tbf, int token){

}

int mytbf_destroy(mytbf_t * tbf){
    struct mytbf_st *me = tbf;   //强转

    job[me->pos] = NULL;
    free(tbf);
    return 0;

}