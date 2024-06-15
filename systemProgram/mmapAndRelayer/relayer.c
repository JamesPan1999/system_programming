#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <math.h>
#include <sys/select.h>
#include <signal.h>
#include <sys/uio.h>

#define TTY1 "/dev/tty11"
#define TTY2 "/dev/tty12"
#define BUFSIZE 1024

int max(int a, int b){
    return a > b ? a : b;
}

void signal_callback(int sig) {
    printf("Hello\n");
    // 如果需要，可以在这里添加更多的处理逻辑
}

// 有限状态机的状态枚举
enum {
    STATE_R = 1, // 读
    STATE_W, // 写
    STATE_AUTO,
    STATE_Ex, // 异常
    STATE_T // 终止
};


struct fsm_st {
    int state;
    int sfd;
    int dfd;
    int len; // 读取到的数据
    int pos; // buf的偏移量
    char buf[BUFSIZE]; // 缓冲区
    char *errstr; // 报错信息
};

/* 状态机驱动 */
static void fsm_driver(struct fsm_st *fsm) {
    int ret;
    // 根据当前状态，决定下一步动作
    switch(fsm->state) {
        // 当前状态为读状态
        case STATE_R:
            fsm->len = read(fsm->sfd, fsm->buf, BUFSIZE);
            if(fsm->len == 0) { // 读完文件
                fsm->state = STATE_T;
            }else if(fsm->len < 0) {
                if(errno == EAGAIN) { // 数据没有准备好
                    fsm->state = STATE_R;
                } else { // 真错
                    fsm->errstr = "read()";
                    fsm->state = STATE_Ex;
                }
            }else { // 转换为写状态
                fsm->pos = 0;
                fsm->state = STATE_W;
            }
            break;
        // 当前状态为写状态
        case STATE_W:
            ret = write(fsm->dfd, fsm->buf + fsm->pos, fsm->len);
            if(ret < 0) {
                if(errno == EAGAIN) {
                    fsm->state = STATE_W;
                } else {
                    fsm->errstr = "write()";
                    fsm->state = STATE_Ex;
                }
            }else {
                fsm->pos += ret;
                fsm->len -= ret;
                if(fsm->len == 0) { // 写够len个字节
                    fsm->state = STATE_R;
                } else { // 没有写够len个字节
                    fsm->state = STATE_W;
                }
            }
            break;
        case STATE_Ex:
            perror(fsm->errstr);
            fsm->state = STATE_T;
            break;
        case STATE_T:
            break;
        default:
            abort();
            break;
    }
}

static void relay(int fd1, int fd2) {
    int fd1_save, fd2_save;
    struct fsm_st fsm12, fsm21;
    fd_set rset, wset;
    // 获取文件状态选项
    fd1_save = fcntl(fd1, F_GETFL);
    // 设置文件状态选项，添加非阻塞模式
    fcntl(fd1, F_SETFL, fd1_save|O_NONBLOCK);

    fd2_save = fcntl(fd2, F_GETFL);
    fcntl(fd2, F_SETFL, fd2_save|O_NONBLOCK);
    // 设置状态机
    fsm12.state = STATE_R;
    fsm12.sfd = fd1;
    fsm12.dfd = fd2;

    fsm21.state = STATE_R;
    fsm21.sfd = fd2;
    fsm21.dfd = fd1;

    while(fsm12.state != STATE_T || fsm21.state != STATE_T) {
        // 布置监视任务，清空fd_set
        FD_ZERO(&rset);
        FD_ZERO(&wset);
        if(fsm12.state == STATE_R) { // 当前为读态，即读sfd
            FD_SET(fsm12.sfd, &rset); // 我们关心sfd是否可读，因此将其加入到rset中
        }
        if(fsm12.state == STATE_W) { // 当前为写态，即写dfd
            FD_SET(fsm12.dfd, &wset);// 我们关心dfd是否可写，因此将其加入到wset中
        }
        if(fsm21.state == STATE_R) {
            FD_SET(fsm21.sfd, &rset);
        }
        if(fsm21.state == STATE_W) {
            FD_SET(fsm21.dfd, &wset);
        }
        // 监视
        if(fsm12.state < STATE_AUTO || fsm21.state < STATE_AUTO) {
            // 阻塞监视
            if(select(max(fd1, fd2) + 1, &rset, &wset, NULL, NULL) < 0) {
                if(errno == EINTR) { // 阻塞调用被信号打断，假错
                    printf("EINTR啊哈哈哈\n");
                    fflush(NULL);
                    continue;
                }
                perror("select()"); // 真错
                exit(1);
            }
        }
        // 查看监视结果
        if(FD_ISSET(fd1, &rset) || FD_ISSET(fd2, &wset) || fsm12.state > STATE_AUTO) { // 读fd1准备好 或 写fd2准备好 或 其他状态
            fsm_driver(&fsm12);
        }
        if(FD_ISSET(fd2, &rset) || FD_ISSET(fd1, &wset) || fsm21.state > STATE_AUTO) { // 读fd2准备好 或 写fd1准备好 或 其他状态
            fsm_driver(&fsm21);
        }
    }

    // 恢复用户设置的文件状态
    fcntl(fd1, F_SETFL, fd1_save);
    fcntl(fd2, F_SETFL, fd2_save);
}


// 模拟用户的设置
int main(void) {
    printf("myPID = %d\n", getpid());
    signal(SIGINT, signal_callback);
    int fd1, fd2; // 左设备和右设备
    // 假设用户不以非阻塞的方式打开文件
    if((fd1 = open(TTY1, O_RDWR)) < 0) {
        perror("open()");
        exit(1);
    }
    write(fd1,"TTY1\n",5);
    // 假设用户以非阻塞的方式打开文件
    if((fd2 = open(TTY1, O_RDWR|O_NONBLOCK)) < 0) {
        perror("open()");
        exit(1);
    }
    write(fd2,"TTY2\n",5);
    relay(fd1, fd2);

    close(fd2);
    close(fd1);



    exit(0);
}
