#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
 
int main(int argc, char *argv[]){
    if(argc < 2){
        fprintf(stderr, "Usage getline_test path...\n");
        exit(1);
    }

    FILE *fp;
    fp = fopen(argv[1], "r");
    if(fp == NULL){
        perror("fopen()");
        exit(1);
    }
    
    char *lineptr = NULL;    //初始化作为标记，第一次进入时用malloc  后面用relloc   
    size_t n = 0;

    while(1){
        if(getline(&lineptr, &n, fp) < 0){    //注意从实际意义而言不是'二级指针'，而是传入'数组的地址'。因为要动态分配内存，所以'数组的地址'会一直发生变化
                                                                         //为什么不用-1？因为为了完整控制所有可能出现的结果，养成好习惯，所有分支可控
            break;
        }
        printf("%ld\n", strlen(lineptr));     //此处用sizeof将得到lineptr指针的大小，即8字节。面试题常考
        printf("%ld\n", n);                              //会relloc   用本.c文件测试，会发现一开始三120  后来变成240
    }
    //getline_free()    //由于种种原因 标准库没有实现，实际上底层应该用free来完成。本处为可控的内存泄漏。

    exit(0);
}