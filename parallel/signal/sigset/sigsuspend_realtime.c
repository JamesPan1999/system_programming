#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>


void int_handler(int sig){
    write(1,"!",1);
}

int main(){

    sigset_t set, oset, saveset;
    sigemptyset(&set);
    sigaddset(&set, SIGRTMIN+6);
    signal(SIGRTMIN+6, int_handler);
    sigprocmask(SIG_UNBLOCK, &set, &saveset); //进入模块前保存原来的状态
    sigprocmask(SIG_BLOCK, &set, &oset);
    printf("pid=%d\n", getpid());
    for(int j = 0; j < 1000; j++){
        for(int i = 0; i < 5; i++){
            write(1,"*",1);
            sleep(1);
        }
        write(1,"\n",1);
        sigsuspend(&oset);//等效于下面的原子操作
       
    //    sigset_t tmpset;
    //    sigprocmask(SIG_SETMASK, &oset, &tmpset);
    //    pause();
    //    sigprocmask(SIG_SETMASK, &tmpset, NULL);
       
    }
    sigprocmask(SIG_SETMASK, &saveset, NULL);   
    exit(0);
}