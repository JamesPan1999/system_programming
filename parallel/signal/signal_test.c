#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static void handler(int sig){
    write(1,"!",1);
    return;
}
int main(int argc,char **argv){
    int i = 0;
    // signal(SIGINT, SIG_DFL); /*SIG_IGN忽略信号 signal函数有用的前提是程序还在运行*/
    signal(SIGINT, handler);
    printf("pid=%d\n", getpid());
    for(i = 0; i < 50; i++){
        // printf("i = %d ",i);
        // fflush(stdout);
        write(1,"*",1);
        sleep(1);
    }
    exit(0);
}