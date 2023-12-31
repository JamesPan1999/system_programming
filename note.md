# 标准IO 

## fopen

### FILE  

FILE  一个管理文件的结构体类型。暂时不需要深究里面到底有什么

### fopen基本用法

man fopen查询

``` c
#include <stdio.h>
FILE *fopen(const char *pathname, const char *mode);
           路径字符串，读写模式
```

RETURN VALUE:
       Upon successful completion fopen(), fdopen()  and  freopen()  return  a FILE pointer.  Otherwise, NULL is returned and <u>errno</u> is set to indicate  the error.

什么是<u>errno</u>？error number   ==可以理解为全局变量（一个指针，我也看不懂了）==，所有地方都可以用。正因为所有出错都会往这个变量里写值，所以应该设计程序：一旦出错立刻打印  （也是最开始设计的一个缺陷）

在/usr/include/asm-generic下可以看到error.h和error-base.h中  errno的值的红定义列表

errno在/usr/include/errno.h中声明  \# define      errno       (*__errno_location ())

​	 ==李老师说errno是线程私有宏？是什么概念？(*__errno_location ())==

编译现象是  如果不包含头文件errno.h，编译显示未定义errno的错误(undeclared)

errno似乎可以被当成一个int类型的全局变量来使用

> 代码示例

```c
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>      //包含errno的定义   第归包含了各种宏值
int main(void) {
    FILE *fp;
    fp = fopen("tmp", "r");
    if(fp == NULL) {
        fprintf(stderr, "fopen() failed! errno = %d.\n", errno);
        exit(1);
    }
    puts("OK!");
    exit(0);
}
```

> > 执行输出：fopen() failed! errno = 2.  （因为文件夹下面没有定义tmp,但用r模式打开)

> /usr/include/asm-generic/errno_base.h中的部分内容

```c
#define EPERM 1 /* Operation not permitted */
#define ENOENT 2 /* No such file or directory */
#define ESRCH 3 /* No such process */
#define EINTR 4 /* Interrupted system call */
#define EIO 5 /* I/O error */
#define ENXIO 6 /* No such device or address */
#define E2BIG 7 /* Argument list too long */
```

### perror

利用perror省去了打印errno之后还要去查宏定义的麻烦，一旦errno这个全局变量产生错误码，就立刻执行这个函数

```c
SYNOPSIS
       #include <stdio.h>   //使用perror需要包含的头文件

       void perror(const char *s);
//--------------------------------------------------------
       #include <errno.h> 
  		  //perror中使用到的一些全局变量，所以也要包含<errno.h>
       const char * const sys_errlist[];
       int sys_nerr;
       int errno;       /* Not really declared this way; see errno(3) */
```

```c
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>      //包含errno的定义   第归包含了各种宏值
int main(void) {
    FILE *fp;
    fp = fopen("tmp", "r");
    if(fp == NULL) {
       //fprintf(stderr, "fopen() failed! errno = %d.\n", errno);    
        perror("fopen()");    //输出 fopen(): +报错内容+回车
        exit(1);
    }
    puts("OK!");
    exit(0);
}
```

> > 输出      No such file or director     （不再需要找errno的数字对应什么了）

perror的好处是能够自动关联全局变量errno

### strerror

```c
//函数原型
/*
*功能：将某个错误代码转换成对应的错误信息
*/
char *strerror(int errnuum);
```

```c
fprintf(stderr, "fopen:%s\n", strerror(errno));  //输出和perror一样
```

### 关于fopen返回的FILE 指针所指的内存空间所存在的地方的讨论

问题：指针指向的内存空间是哪一块（或者说FILE结构体放在内存的哪一块）？是堆，是栈，还是静态区？

* 栈

  ```c
  // 简单的fopen源码分析
  FILE *fopen(const char *filename, const char *mode) {
      FILE tmp;
      
      // 给结构体成员赋值初始化
      tmp.xxx = xxx;
      tmp.yyy = yyy;
      ...
          
      return &tmp;
  }
  ```

  显然，局部变量tmp在离开函数后，就被释放了，这种方式错误。最终返回主程序的指针是一个未被分配的内存区域

* 静态区（是一种方法，有些库函数会使用，比如查看系统时间之类的，但fopen不行）

  ```c
  // 简单的fopen源码分析
  FILE *fopen(const char *filename, const char *mode) {
      static FILE tmp;
      
      // 给结构体成员赋值初始化
      tmp.xxx = xxx;
      tmp.yyy = yyy;
      ...
          
      return &tmp;
  }
  ```

  加上static，将tmp保存在静态区（静态无链接），但是只能存在一个FILE实例（因为打开文件调用的都是fopen这个函数，而FILE 如果定义为一个全局变量，将被存在静态区，每次调用fopen将会使得静态变量的前一次的内容被覆盖）；例如：

  ```c
  fp1 = fopen("a", "r");
  fp2 = fopen("b", "r");
  // 此时fp1实际指向了b，第二次的结果会把第一次的结果覆盖掉
  ```

* 堆 （正确，有申请空间）

  ```c
  // 简单的fopen源码分析
  FILE *fopen(const char *filename, const char *mode) {
      FILE *tmp = NULL;
      tmp = malloc(sizeof(FILE));   //有malloc一定要有free  互逆操作成对存在。所以free就在fclose中 可以举一反三通过这个技巧判断其他函数的情况
      
      // 给结构体成员赋值初始化
      tmp->xxx = xxx;
      tmp->yyy = yyy;
      ...
          
      return tmp;
  }
  
  ```

  此时变量tmp具有动态存储期，从调用malloc分配内存到调用free释放内存为止，而free就在fclose函数中被调用。


## fclose 

C 库函数 fclose 关闭流 stream。刷新所有的缓冲区。

```c
int fclose(FILE *stream)
```

* stream — 这是指向 FILE 对象的指针，该 FILE 对象指定了要被关闭的流。
* 如果流成功关闭，则该方法返回零。如果失败，则返回 EOF(值为-1)。

```c
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(void) {
    FILE *fp;
    fp = fopen("tmp", "r");
    if(fp == NULL) {
        perror("fopen()");
        exit(1);
    }
    puts("OK!");
    fclose(fp); // 释放内存     一般都会关闭成功，所以一般不检查返回值
    exit(0);
}
```

## 一个进程中能够打开流的最大个数

```c
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(void) {
    int count = 0;
    FILE *fp = NULL;
    
    while(1){
        fp = fopen("tmp", "r");   //不断开流    （文件即一种流）
        if(fp == NULL) {
            perror("fopen()");   //自动读取errno的内容
            break;
        }
        count++;
     }  
    printf("%d\n",count);     //发现结果是1021  
    exit(0); 
}
```

> > 在不更改当前环境的前提下，一个进程默认已经打开了3个流：stdin stdout stderr

``` shell 
ulimit -a        可以看到open files  为1024   可以通过  ulimit -u 1024来更改
```

## 打开文件不存在后，新建文件的默认权限问题

如果用w或者a新建文件，那么这个文件的权限是什么？--664   为什么不是666或者其他？

公式：0666  &  ~umask

linux下 umask 的默认值是0002        (0开头都是八进制) 

所以最后按照公式得到的结果权限就是664    

umask越大，所创建的权限越小      umask用于权限控制   

## 零碎知识点

### 1. 使用man手册中的函数时，一定要记得包含它所要求的所有头文件（比如stdlib.h等）

```c
//在使用malloc时很多同学这样错误地使用
int *p = (int *)malloc(sizeof(int));  //void->int强制转换  这是在掩盖问题，而不是解决问题

//正确做法 是加上头文件
#include <stdlib.h>   //void隐式转成任意类型应该是天经地义的
										//如果不包含头文件，gcc在对程序进行编译时，也就不知道malloc的定义是什么
										// 依据c标准，对于没有定义的函数，编译器默认认为它返回的参数是int类型的，和int*不匹配，所以报警告
int *p = malloc(sizeof(int));

```

ps:会当gcc编译器编译到一个在此之前没有定义和声明的函数，并不会直接中断编译，而是会隐式声明一个同名的函数，这个隐式声明的函数默认返回值类型为int 。

### 2. puts("字符串")

纯粹为了方便，可以避免printf复杂的语法。该函数碰到\0就结束，并且额外输出了一个换行符

### 3. stderr stdout 结合fprintf

在C语言编程中，fprintf函数的主要功能是格式化输出数据到流。当我们将stderr作为fprintf函数的第一个参数输入时，意味着我们将要输出的错误信息通过标准错误输出设备（通常是屏幕）打印出来。例如，我们可以用以下方式打印一条错误信息到stderr：

```c
fprintf(stderr, "%s", "Stack overflow!"); // Error message on stderr (using fprintf)
```

这条语句会在屏幕上显示"Stack overflow!"这样的错误信息。值得注意的是，与stdout不同，stderr是无缓冲的，意味着其输出会直接显示，不会像stdout那样放在一个buffer里面等待换行时才输出。此外，我们还可以通过代码将stderr重定向到文件或磁盘，或者重写为标准输出设备。

stderr、stdout和stdin这三者都是指向与标准输入流、标准输出流、标准错误流相关联的文件句柄，它们是程序自身的文件句柄，不是普通的文件，而是设备文件。还需要注意的是，尽管默认情况下stdout和stderr的输出都是向屏幕的，但它们也可以被重定向到磁盘文件。例如，可以将标准输出重定向到磁盘文件，然后可以看出stdout和stderr的区别：stdout的输出会被保存到磁盘文件中，而stderr的输出依然在屏幕上。