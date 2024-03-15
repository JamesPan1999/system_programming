#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(){

    alarm(10);
    alarm(1);
    alarm(10);  //alarm永远按照最后一次时间设置来执行
    // pause();

    while(1){
        printf("pause\n");
        //sleep(1);  sleep 在一些平台上的本质是alarm+pause  然而这将导致sleep中的alarm和用户自己写的alarm冲突导致定时不准
        pause();   //这个while循环只执行了一次，因为alarm(1)设置了1秒的闹钟，当闹钟到了，会自动执行pause()函数，pause()函数会阻塞当前进程，让进程进入休眠状态，直到收到信号为止
    }
    exit(0);
}