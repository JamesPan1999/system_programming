#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]){

    puts("Begin");

    fflush(NULL);   //刷新缓冲区

    execl("/bin/date", "date", "+%s", NULL);    //从argv[0]开始传，多以第一个是date
    perror("execl()");     //如果能会的来其实一定出错了

    puts("End");


    exit(1);   //能回来一定出错了

}