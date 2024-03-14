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

static volatile int token = 0;
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

        while(token <= 0){
            pause();
        }
        token--;   //消耗掉一个令牌

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
