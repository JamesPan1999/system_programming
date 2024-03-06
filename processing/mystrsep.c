#include <stdlib.h>
#include <stdio.h>
#define DELIMS " \t\n"

/**
 * 分割字符串。
 * 
 * 该函数使用指定的分隔符将一个字符串分割成多个子字符串。每次调用都会返回一个指针，
 * 指向下一个未被分隔符分割的部分。当遍历完整个字符串后，函数返回NULL。
 * 
 * @param stringp 指向要被分割的字符串的指针的地址。在函数返回后，这个指针会指向下一个
 *                待分割字符串的起始位置，或者为NULL，表示没有剩余字符串可供分割。
 * @param delim 分隔符字符串。该函数会使用这个字符串中的任意字符作为分隔符来分割输入的字符串。
 * @return 返回一个指向分割出的子字符串的指针。当没有更多的子字符串可供分割时，返回NULL。
 * 在mystrsep函数中，参数stringp被定义为一个指向字符指针的指针（二级指针），这是因为该函数需要修改原始字符串指针所指向的位置，以便在每次分割操作后更新它。
函数的主要目的是将输入的字符串按照指定分隔符进行分割，并返回分割出的第一个子串。同时，在完成对一个子串的处理后，通过改变*stringp的值来指向剩余未处理部分的起始地址，这样下一次调用时就可以从上次结束的地方继续分割。
如果stringp只是一个一级指针，即传入的是字符串的地址而不是其指针的地址，则函数内部无法直接更改这个地址值，也就无法实现在原地连续分割字符串的功能。因此，使用二级指针是为了能够在函数内部修改实参所指向的地址，从而实现分割并遍历整个字符串的目的。
 */
static char *mystrsep(char **stringp, const char *delim)
{
    char *s;
    const char *spanp;
    int c, sc;
    char *tok;
    // 判断待分割的字符串是否为空，若为空，则直接返回NULL
    if ((s = *stringp)== NULL)
        return (NULL);
    for (tok = s;;) { // 无限循环，直到分割完所有可分割的部分或遇到字符串结尾
        c = *s++; // 获取当前检查的字符
        spanp = delim; // 重置分隔符指针，指向分隔符字符串的起始位置
        do {
            // 检查当前字符是否为分隔符
            if ((sc =*spanp++) == c) {
                // 如果当前字符是0，说明到达字符串末尾
                if (c == 0)
                    s = NULL;
                else
                    s[-1] = 0; // 将分隔符替换为0，表示子字符串的结束
                *stringp = s; // 更新*stringp为下一个待分割字符串的起始位置
                return (tok); // 返回找到的子字符串
            }
        } while (sc != 0); // 继续检查分隔符字符串中的下一个字符，直到结束
    }
    /* NOTREACHED */ // 此处代码不可达，用于表示理论上不可能到达的代码段
}

static  void parse(char *linebuf){      
    char *token;
    while(1){
        token = mystrsep(&linebuf, DELIMS);
        if(token == NULL)
            break;
        if(token[0] == '\0'){
            puts(token);
            continue;
        }
        puts(token);
        // puts(linebuf);
    }
}

int main(int arvgc, char * argv[]){

    char *lineptr = NULL ;
    size_t  size_n = 0;

    while(1){
        printf("please enter cmd$");
        if(getline(&lineptr, &size_n, stdin) < 0)
            break;
        parse(lineptr);

    }
    exit(0);
}