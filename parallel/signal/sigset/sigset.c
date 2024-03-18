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
    sigaddset(&set, SIGINT);
    signal(SIGINT, int_handler);
    sigprocmask(SIG_UNBLOCK, &set, &saveset); //进入模块前保存原来的状态

    for(int j = 0; j < 1000; j++){
        // sigprocmask(SIG_BLOCK, &set, &oset); //原来的信号屏蔽集合放在oset中
        sigprocmask(SIG_BLOCK, &set, NULL); //原来的信号屏蔽集合放在oset中
        for(int i = 0; i < 5; i++){
            write(1,"*",1);
            sleep(1);
        }
        write(1,"\n",1);
        // sigprocmask(SIG_SETMASK, &oset, NULL);
        sigprocmask(SIG_UNBLOCK, &set, NULL);  //上下两句话对于这个脚本实际的意义相同
    }
    sigprocmask(SIG_SETMASK, &saveset, NULL);   
    exit(0);
}