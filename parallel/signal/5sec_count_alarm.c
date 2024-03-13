#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

volatile static int loop_flag = 1;
// 编译优化时(即-O1），gcc看到下面的循环体中没有出现loop，所以gcc认为loop是不变的，
// 将loop的值存储在高速缓存（CPU的寄存器）中，每次读取loop的值时，从高速缓存中读取，
// 而不是在loop实际存放的内存地址中读取，因此一直在此处循环。
// 用volatile修饰loop，此时编译器认为该变量易变，每次读取时从实际内存地址中读取：

static void alrm_handle(int sig){
    loop_flag = 0;
}

int main(){
    int64_t count = 0;

    signal(SIGALRM, alrm_handle);
    alarm(5);//signal必须放在alarm之前

    while(loop_flag){
        count++;
    }

    printf("%ld\n", count);

    exit(0);
}