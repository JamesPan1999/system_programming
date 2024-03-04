#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <wait.h>

int main(int argc, char **argv){
    puts("father Begin");
    fflush(NULL);

    pid_t pid = fork();
    if(pid < 0){
        perror("fork()");
        exit(1);
    }
    if(pid == 0){
        execl("/bin/date", "date", NULL);    //子进程金蝉脱壳
        perror("execl()");   //如果能子进程能够执行到这里，则说明一定出错了
        exit(1);
    }
    pid_t pid_return = wait(NULL);
    printf("%d returned and fater end\n", pid_return);
    exit(0);

}
