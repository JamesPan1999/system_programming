#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFSIZE 1024

int main(int argc, char *argv[])
{
    if(argc < 3){
        fprintf(stderr, "Usage...\n");
        exit(EXIT_FAILURE);
    }

    int sfd, dfd;
    char  buf[BUFFSIZE];
    ssize_t len, ret;

    sfd = open(argv[1], O_RDONLY);
    if(sfd < 0){
        perror("source open() error");
        exit(EXIT_FAILURE);
    }

    dfd = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0600);
    if(dfd < 0){
        close(sfd);
        perror("destnation open() error");
        exit(EXIT_FAILURE);
    }

    while (1){
        len = read(sfd, buf, BUFFSIZE);
        if(len < 0){
            perror("read()");
            break;
        }
        if(len == 0){   //要想到，最终读完的时候将只能读到0个字节，不然会陷入死循环   (read 碰到文件尾时结束读取，并返回本次读到的个数)
            break;
        }
        //printf("%ld\n",len);
        
        ret = write(dfd, buf, BUFFSIZE);

    }

    close(dfd);
    close(sfd);

    exit(EXIT_SUCCESS);
}