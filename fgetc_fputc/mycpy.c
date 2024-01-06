#include<stdlib.h>
#include<stdio.h>

int main(int argc, char *argv[]){
    FILE *fps, *fpd;
    // int ch;      

    if(argc < 3){      //第一步判断输入参数个数是否正确     如果不判断，后面fopen可能报错你也摸不着头脑
        printf("Usage: %s <src_file> <dest_file>\n",argv[0]);
        exit(1);
    }

    fps = fopen(argv[1],  "r");   //打开src_file  注意"r"不是" r"   否则执行时错误invalid argument 并且这个错误是在下面的perror下面输出的 
    if(fps == NULL){            //记得判断是否成功，如果不判断会出错，你可能想找错误，但找不到
        perror("fopen1");
        exit(1);
    }


    fpd = fopen(argv[2], "w");   //打开dest_file
    if(fpd == NULL){            //记得判断是否成功，如果不判断会出错，你可能找不到错误，但找不到
        fclose(fps);       //  防止内存泄漏    但这不是个好方法，日后会学到可以用钩子函数实现
        perror("fopen2");
        exit(1);
    }

    while(1){
        // char c = fgetc(fps);   //不建议的类型 char  建议用int  请参考man手册
        int c = fgetc(fps);   //不建议的类型 char  建议用int  请参考man手册   
        //建议不要在循环的栈上定义变量 容易溢出
        if(c == EOF){
            break;
        }
        fputc(c, fpd);
    }

    fclose(fpd);
    fclose(fps);
    exit(0);

}

