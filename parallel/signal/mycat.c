#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#define BUFSIZE 1024

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
    } while(sfd < 0);
	
    while(1) {

        while((len = read(sfd, buf, BUFSIZE)) < 0) {
            if(errno == EINTR)
                continue;
            perror("read()");
            break;
        }
		
        if(len == 0)
            break;

        pos = 0;
        while(len > 0) {
            if((ret = write(dfd, buf + pos, len)) < 0) {
                if(errno == EINTR)
                    continue;
                perror("write()");
                exit(1);
            }
            pos += ret;
            len -= ret;
        }
    }
    close(sfd);
    exit(0);
}
