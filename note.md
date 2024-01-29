

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



## fputc  fgetc

```c   
int fgetc(FILE *stream);     //函数
int getc(FILE *stream);      //宏
//将读到的unsigned char 类型转化为int类型来对待  防止出错（ 失败或文件尾返回EOF (本质为-1)，所以前面必须用int接收。）
//宏为了更快，函数更稳定（函数有类型检查）//执行消耗时间和编译消耗时间的区别
int getchar(void)       // char() is equivalent to getc(stdin) 
```

```c
int fputc(int c, FILE *stream);
int putc(int c, FILE  *stream);
int putchar(int c);
```

```c
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
    //为什么会在下面的perror输出？
    //首先判断这个字符串是否正确，是程序执行之后才能判断的，所以编译不报错。
    //在程序执行的时候，fopen的输入参数错误后，会将输入参数错误原因放到errno中，并且fopen返回NULL
    //程序继续向下执行（注意不会因为这个错误就exit程序），碰到perror后输出的错误将是因为" r"输错而产生的那个错误的记录
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
        fputc(c, fpd);      //一般都能够写入成功，所以不会检验最终是否打开成功
    }

    fclose(fpd);
    fclose(fps);
    exit(0);

}
```

## fgets和fputs    以及    gets和puts

注意这里的f还是指的是file的意思

gets和fgets函数都可以用来读取字符串，但它们在处理输入的方式和安全性上存在显著的区别。首先，gets函数会一直读取输入，直到遇到换行符为止，然后它会丢弃这个换行符并储存其余字符。此外，gets函数在字符末尾添加一个空字符使其成为一个C字符串。然而，由于gets函数不检查输入字符的大小，如果输入的字符大于定义的数组长度，就会导致内存越界，从而可能引发堆栈溢出。

相比之下，fgets函数则更为安全，它可以指定读取的字符个数。具体来说，fgets函数会读取指定数量或指定字节数的字符，或者当遇到换行符时停止读取。与gets函数不同的是，如果fgets函数读到换行符，它会把它存储到字符串中，而不是像gets函数那样丢弃它。因此，fgets函数具有防止缓冲区溢出的特性。

总的来说，由于gets函数存在缓冲区溢出的问题，通常被认为是不安全的，而fgets函数由于其对输入字符个数的严格控制，被广泛认为是一个更安全的选择。

```c
char *gets(char *str); // 从标准输入 stdin 读取一行，并把它存储在 str 所指向的字符串中。当读取到换行符时，或者到达文件末尾时，它会停止，具体视情况而定。
// 如果成功，该函数返回 str。如果发生错误或者到达文件末尾时还未读取任何字符，则返回 NULL。
//--------------------------------------------------------
int puts(const char *str); // 把一个字符串写入到标准输出 stdout，直到空字符，但不包括空字符。换行符会被追加到输出中。
// 如果成功，该函数返回一个非负值为字符串长度（包括末尾的 \0），如果发生错误则返回 EOF。

```

```c
// 从指定的流 stream 读取一行，并把它存储在 str 所指向的字符串内。当读取 (n-1) 个字符时，或者读取到换行符时，或者到达文件末尾时，它会停止，具体视情况而定。
char *fgets(char *str, int n, FILE *stream); 
// 如果成功，该函数返回相同的 str 参数。如果到达文件末尾或者没有读取到任何字符，str 的内容保持不变，并返回一个空指针。
// 如果发生错误，返回一个空指针。
//--------------------------------------------------------
//  把字符串写入到指定的流 stream 中，但不包括空字符。
int fputs(const char *str, FILE *stream);
// 该函数返回一个非负值，如果发生错误则返回 EOF。
```

---

fgets读取结束的条件，满足其一即可：

* 读到size-1个字符时停止，size位置存放\0  (这样设计这个函数是因为要get一个string，后面必须用\0结尾)
* 读到换行符'\n'时停止    
* 读到文件末尾EOF

```c
#define SIZE 5
char buf[SIZE]; // 栈上的动态内存
fgets(buf, SIZE, stream);

如果stream = "abcde"
则buf = "abcd\0"(读到size-1后补\0)，文件中的指针指向e（指针永远指向下一个要读的位置）

如果stream = "ab"        //你打开一个文件后，默认就会有一个换行符在那里（如果是输入流(stdin)的话就没有,且输入流不会检测size，溢出了也不会管）
则buf = "ab\n\0"(读到换行符)，文件指针指向EOF

极端的情况：
如果stream = "abcd"
则需要fgets读取两次才能读完
第一次读取的为"abcd\0"(读到SIZE-1)，指针指向'\n'
第二次读取的为"\n\0"(读到换行符)，指针指向EOF
```

```c 
#include <stdio.h>
#include <stdlib.h>
#define SIZE 1024

int main(int argc, char **argv) {

    FILE *fps, *fpd;
    char buf[SIZE];

    if(argc < 3) {
        fprintf(stderr, "Usage:%s <src_file> <dest_file>\n", argv[0]);
        exit(1);
    }

    fps = fopen(argv[1], "r");
    if(fps == NULL) {
        perror("fopen()");
        exit(1);
    }

    fpd = fopen(argv[2], "w");
    if(fpd == NULL) {
        fclose(fps);
        perror("fopen()");
        exit(1);
    }

    while(fgets(buf, SIZE, fps) != NULL)
            fputs(buf, fpd);

    fclose(fpd);
    fclose(fps);

    exit(0);
}
```

## fread和fwrite

fread从给定流 stream 读取数据到 ptr 所指向的数组中。

```c
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
ptr — 这是指向带有最小尺寸 size*nmemb 字节的内存块的指针。
size — 这是要读取的每个元素的大小，以字节为单位。
nmemb — 这是元素的个数，每个元素的大小为 size 字节。
stream — 这是指向 FILE 对象的指针，该 FILE 对象指定了一个输入流。

成功读取的元素总数会以 size_t 对象返回，size_t 对象是一个整型数据类型。如果总数与 nmemb 参数不同，则可能发生了一个错误或者到达了文件末尾。
```

fwrite把 ptr 所指向的数组中的数据写入到给定流 stream 中。

```c
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
ptr — 这是指向要被写入的元素数组的指针。
size — 这是要被写入的每个元素的大小，以字节为单位。
nmemb — 这是元素的个数，每个元素的大小为 size 字节。
stream — 这是指向 FILE 对象的指针，该 FILE 对象指定了一个输出流。
如果成功，该函数返回一个 size_t 对象，表示元素的总数，该对象是一个整型数据类型。如果该数字与 nmemb 参数不同，则会显示一个错误。
```

```c
fread(buf, size, nmemb, fp);

// 情况1：数据量足够
// 情况2：文件只有5个字节

// 读10个对象，每个对象1个字节
fread(buf, 1, 10, fp);

// 情况1：
// 第一次读：返回10（读到10个对象），读到10个字节
// 情况2：
// 第一次读：返回5（读到5个对象），读到5个字节

//--------------------------------

// 读1个对象，每个对象10个字节
fread(buf, 10, 1, fp);

// 情况1：
// 第一次读：返回1（读到1个对象），也读到10个字节
// 情况2：
// 第一次读：返回0（读不到1个对象，因为1个对象要10字节，而文件只有5个字节）

```

```c
#include <stdio.h>
#include <stdlib.h>
#define NNUMB 1024

int main(int argc, char **argv) {

    FILE *fps, *fpd;
    char buf[NNUMB];
    int n;

    if(argc < 3) {
        fprintf(stderr, "Usage:%s <src_file> <dest_file>\n", argv[0]);
        exit(1);
    }

    fps = fopen(argv[1], "r");
    if(fps == NULL) {
        perror("fopen()");
        exit(1);
    }
    
    fpd = fopen(argv[2], "w");
    if(fpd == NULL) {
        fclose(fps);
        perror("fopen()");
        exit(1);
    }
	
    // 如果成功读到n(n>0)个对象，则返回n
    // 将这n个对象写入流中
    //注意  这里必须判断读到了多少个块    如果失败或者读到的块的个数小于实际个数，会出问题
    while((n = fread(buf, 1, NNUMB, fps)) > 0)      // 一次只读一个字节数据   读到了多少个对象就写多少个对象
            fwrite(buf, 1, n, fpd);    		       						// 一次只写一个字节数据
    
    //while(fread(buf, 1, NNUMB, fps))    //错误
    //   	fwrite(buf, 1, NNUMB, fpd);   
    
    
    fclose(fpd);
    fclose(fps);

    exit(0);
}
```

## fseek 和 ftell

这是两个定义的比较丑的函数

fseek：设置流 stream 的文件位置为给定的偏移 offset，参数 offset 意味着从给定的 whence 位置查找的字节数。

int fseek(FILE *stream, long int offset, int whence)

    stream — 这是指向 FILE 对象的指针，该 FILE 对象标识了流。
    offset — 这是相对 whence 的偏移量，以字节为单位。
    whence — 这是表示开始添加偏移 offset 的位置。它一般指定为下列常量之一：

| 常量     | 描述               |
| -------- | ------------------ |
| SEEK_SET | 文件的开头         |
| SEEK_CUR | 文件指针的当前位置 |
| SEEK_END | 文件的末尾         |

    如果成功，则该函数返回零，否则返回非零值。

ftell：返回给定流 stream 的当前文件位置。

long int ftell(FILE *stream)

```
stream — 这是指向 FILE 对象的指针，该 FILE 对象标识了流。
该函数返回位置标识符的当前值。如果发生错误，则返回 -1L，全局变量 errno 被设置为一个正值
```

---

```c
int fseek(FILE *stream,  long offset, int whence);
//offset：需要偏移的字节数，用于设置新的读/写位置；
//origin：基准点，用来决定从哪个位置开始偏移。
// 问题在于，long在不同的平台上长度不同  比如在32位机器上是占32位字节。范围是  -2G-2G，一共有4G的寻址范围
long ftell(FILE *stream);
//找当前文件的指针在哪，返回当前文件的当前指针指向的位置
//返回用long 则范围是 -2G-2G  实际上返回的指针的位置不可能是负数，所以实际范围是0-2G  导致了内存的浪费
```

当fseek和ftell组合使用的时候，fseek的4G 范围将会被浪费掉

这是一个历史遗留问题，并且这个设计是古人的设计思维，没有考虑到现在有很多视频文件远远大于4G 

为了填这个坑用fseeko和ftello代替

---

void rewind(FILE *stream);   让指针回到文件头

---

fseek可以产生空洞文件 ，就是很多\0构成的文件

## fseeko 和 ftello

fseek和ftell中偏移offset的修饰类型是long，因此只能对2G左右大小的文件进行操作，否则会超出long的范围；

fseeko和ftello则将偏移的修饰类型使用typedef定义为offset_t，具体类型交由系统决定，因此不存在文件大小的限制。但是这两个函数不是C标准库函数，而是隶属于POSIX标准（POSIX是标准C库的超集，或者说，C库是普通话，而POSIX是方言）。

## fflush

```c
#include<stdlib.h>
#include<stdio.h>

int main(){
    printf("before while()");    //行缓冲模式，只有碰到\n的时候或者是一行输入满的时候，才会向屏幕输出  
    // printf("before while()\n");   //加上\n就可以输出了  
     while(1){}
    printf("after while()");
    exit(0);
}
```

以上程序会卡在while并且不会输出before

fflush用于将缓冲区的内容刷到屏幕上

```c
#include<stdlib.h>
#include<stdio.h>

int main(){
    printf("before while()");   
    fflush(stdout);
     while(1){}
    printf("after while()");
    fflush(NULL);   //If the stream argument  is  NULL,  fflush()  flushes  all  open  output streams.
    exit(0);
}
```

缓冲区的作用：大多数情况下是好事，合并系统调用，增加程序的吞吐量。

缓冲的分类：

    行缓冲line buffered：针对标准输出（屏幕等终端设备），有换行刷新，缓冲满刷新，强制刷新(fflush)三种，后两个和全缓冲一致；
    全缓冲fully buffered：默认缓冲机制（除标准输出【终端设备】，例如重定向到文件），有缓冲满刷新，强制刷新两种，强制刷新例如调用fflush函数，或者进程结束时也会强制刷新；此时换行符仅仅只是个换行符，没有刷新功能；
    无缓冲unbuffered：例如stderr，需要立即输出，数据会立即读入内存或者输出到外存文件和设备上；

setvbuf：定义流 stream 应如何缓冲。理解即可。

int setvbuf(FILE *stream, char *buffer, int mode, size_t size)

    stream — 这是指向 FILE 对象的指针，该 FILE 对象标识了一个打开的流。
    buffer — 这是分配给用户的缓冲。如果设置为 NULL，该函数会自动分配一个指定大小的缓冲。
    mode — 这指定了文件缓冲的模式：

模式 	描述
_IOFBF 	全缓冲：对于输出，数据在缓冲填满时被一次性写入。对于输入，缓冲会在请求输入且缓冲为空时被填充。
_IOLBF 	行缓冲：对于输出，数据在遇到换行符或者在缓冲填满时被写入，具体视情况而定。对于输入，缓冲会在请求输入且缓冲为空时被填充，直到遇到下一个换行符。
_IONBF 	无缓冲：不使用缓冲。每个 I/O 操作都被即时写入。buffer 和 size 参数被忽略。

## getline

封装了malloc  动态扩充内存的一系列步骤。从流中获得一行字符

#define _GNU_SOURCE // 通常将这种宏写在makefile中，现在的编译器没有了该宏，直接使用即可  (声明了是c的GNU标准库)
#include <stdio.h>
ssize_t getline(char **lineptr, size_t *n, FILE *stream);

为什么要用二级指针？是因为C语言在进行函数调用的时候,是将实参的值复制一份,并将其副本传递到函数调用里,如果形定义的不是指针,那么在函数内部改变数值,不会对实参本来的值发生改变。而将形参定义成了指针的话,那么传到函数里面的值虽然是实参地址的一个副本,但是地址里存的值发生了改变,也就导致实参本来的值也发生了改变。有了上述分析的基础上,我们知道,如果要在一个函数内改变一个数的值,那么就需要将形参定义为指针。同样的,如果我们要在一个函数内改变一个指针的值,我们就需要将形参定义了二级指针

```c
int main(int argc, char **argv) {
    FILE *fp;
    // 一定要初始化，否则指针会指向内存中的随机位置
    char *linebuf = NULL;
    size_t linesize = 0;
    if(argc < 2) {
        fprintf(stderr, "Usage...\n");
    }
    fp = fopen(argv[1], "r");
    if(fp == NULL) {
        perror("fopen()");
        exit(1);
    }
    while(1) {
        // 当返回-1时则读完
    	if(getline(&linebuf, &linesize, fp) < 0)
            break;
       	printf("%d\n", strlen(linebuf));   //可以看到linebuf的长度是不断变化的，说明函数内部修改了linebuf
    }
    fclose(fp);
    exit(0);
}
```




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

stderr、stdout和stdin这三者都是指向与标准输入流、标准输出流、标准错误流相关联的文件句柄，它们是程序自身的文件句柄，不是普通的文件，而是设备文件。还需要注意的是，尽管默认情况下stdout和stderr的输出都是向屏幕的，但它们也可以被重定向到磁盘文件。例如，可以将标准输出重定向到磁盘文件

### 4. atoi

```c 
#include <stdio.h>
#include <stdlib.h>

int main()
{
    //-------------ASCII to int ----------------
    char str[] = "123a456";
    printf("%d\n", atoi(str));    //atoi 转化到a之前为止
    //-------------int to ASCII-----------------本身没有这样的函数，但用sprintf可以完全替代
   	char buf[1024];
    int year = 2024, month = 1, day = 17;
    sprintf(buf, "%d-%d-%d", year, month, day);
    puts(buf);
    exit(0);
}
```

sprintf存在一个问题，就是不知道buf到底有多大，容易溢出；为此引入了snprintf

注意snprintf中的size参数，实际上只能放size-1个用户字符，因为最后一个留给\0

