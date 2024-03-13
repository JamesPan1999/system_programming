#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    char *file = argv[1];
    FILE *fp = fopen(file, "r");
    if (fp == NULL) {
        printf("Error: file %s not found\n", file);
        return 1;
    }
    
    FILE *fp2 = fopen(file, "r");

    char buf[100] = "\0";
    char buf2[100] = "\0";

    fread(buf, 1, 10, fp);
    puts(buf);
    fread(buf2, 1, 1, fp2);
    puts(buf2);
    printf("%d\n", fp2->_fileno);   //fd文件描述符
    printf("%d\n", fp->_fileno);

    printf("%s\n", fp2->_IO_read_ptr);  //调fread的个数看数值增量
    printf("%s\n", fp->_IO_read_ptr);
    //每次调用fread都会产生新的FILE结构体，并且FLIE中当前指针（_IO_read_ptr）指向文件的开头，本质上上面open出来的两个FILE 结构体是完全独立的
    //其底层的文件描述符fd所指向的结构体也是两个完全独立的结构体，所以相互的位置操作不会影响

    fclose(fp);
    return 0;
}