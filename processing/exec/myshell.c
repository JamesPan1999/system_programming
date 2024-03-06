#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <wait.h>
#include <glob.h>
#include <string.h>

#define DELIMS   " \n\t"
struct cmd_st{    //增强程序的可扩展性 所以用了一个结构体
    glob_t glob;
};


static void prompt(void){
    printf("myshell-please-enter...$");
    fflush(stdout);
}

static  void parse(char *linebuf, struct cmd_st *cmd){      //这里没有设计成返回glob_t类型而是cmd_st类型也是为了程序的可扩展性
    char *token;
    int i = 0;      //不加会有段错误  
    while(1){
        token = strsep(&linebuf, DELIMS);
        if(token == NULL)
            break;
        if(token[0] == '\0')
            continue;
        // puts(token);
        // puts(linebuf);
        if(i == 0){
            glob(token, GLOB_NOCHECK, NULL, &(cmd->glob));
            i = 1;   //首次进入时      李慧芹的课上用位图的方式去实现   效率更高但初学者读不懂
                        //如果首次进入时直接使用GLOB_APPEND则此时还没分配变量glob.gl_pathv的储存空间，此时将会出段错误
        }else{
             glob(token, GLOB_NOCHECK|GLOB_APPEND, NULL, &(cmd->glob));
        }
      
    }
}

int main(){
    
    pid_t  pid;
    //getline
    char *lineptr = NULL;
    size_t bufsize = 0;
    struct cmd_st cmd_res;

    while(1){
        prompt();   //弹出shell提示 s
        
        if(getline(&lineptr, &bufsize, stdin) < 0)   //获取命令  如果什么都没输入
            break;

        // puts(lineptr);
        parse(lineptr, &cmd_res);      //命令分离

        // for (int i = 0; i < cmd_res.glob.gl_pathc; i++){
        //     printf("%s\n", cmd_res.glob.gl_pathv[i]);
        // }

        if (0) {      //内部命令

        }else //外部命令
        {
            pid = fork();
            if(pid < 0){
                perror("fork()");
                exit(1);
            }
            if(pid == 0){    //子进程
                execvp(cmd_res.glob.gl_pathv[0], cmd_res.glob.gl_pathv);     // 用那种exec？这里最佳是结合glob使用argv的形式的那种，因为不可能事先知道到底输入几个arg，
                                        //而且系统已经帮你实现了可以变内存长度的动态储存类型glob_t
                                        //至于我们一般不会输入/bin/pwd类似的命令而采用直接输入pwd，所以用了execvp 而非execv
                perror("exec()"); //能执行到这里必然出错，如果execxxx正常执行则这里不可能走到
                exit(1);
            }else{
                wait(NULL);
                globfree(&cmd_res.glob);
            }
        }
    }
    exit(0);

}