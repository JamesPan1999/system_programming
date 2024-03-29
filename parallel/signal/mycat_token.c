#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#define CPS 10   //每秒钟写10个字
#define BUFSIZE CPS 
#define BURST 100  //令牌筒大小上限

// static volatile int token = 0;   //不原子
static volatile sig_atomic_t token = 0;   //原子
static void alrm_handler(int sig) {
    alarm(1);
    token++;
    if(token > BURST)
        token = BURST;
}

int main(int argc, char **argv) {
    
    int sfd, dfd = 1;
    char buf[BUFSIZE];
    int len, ret, pos;

    if(argc < 2) {
        fprintf(stderr, "Usage...\n");
        exit(1);
    }


    do {
        if((sfd = open(argv[1], O_RDONLY)) < 0) {
            if(errno != EINTR) {
                perror("open()");
                exit(1);
            }
        }
    } while(sfd < 0);    //do-while也有if+重新执行的语义
	
    signal(SIGALRM, alrm_handler);
    alarm(1);

    while(1) {

        while(token <= 0){ //一个小问题：这句话和下面这句之间来了一个alarm将会丢掉这个，不过因为是令牌同所以下一次来的时候会处理2个token
            pause();
        }
        token--;   //消耗掉一个令牌
                   //token--不是由一条机器指令来完成的，不原子，可能会和alrm_handler中的token冲突

        while((len = read(sfd, buf, BUFSIZE)) < 0) {  //while有if+重新执行的语义
            if(errno == EINTR)    //所有的系统调用都可能被信号中断而返回EINTR的错误码，此时只是一个假错误，重新执行read一般可以解决
                continue;
            perror("read()");
            break;
        }
		
        if(len == 0)
            break;

        //用pos避免write被意外打断时，实际没有写完len个字节的情况
        pos = 0;
        while(len > 0) {
            if((ret = write(dfd, buf + pos, len)) < 0) {
                if(errno == EINTR)
                    continue;   //重构程序是要注意continue
                perror("write()");
                exit(1);
            }
            pos += ret;
            len -= ret;
        }
        // sleep(1);

    }
    close(sfd);
    exit(0);
}
