#ifndef MYTBFS_H__
#define MYTBFS_H__

#define MYTBF_MAX  1024  //这是模块相关的内存大小控制，所以放在mytbf.h中进行定义，而cps和burst我们希望由用户层面灵活指定，所以就放到了main中；
                         //理论上，cps和burst是运行时才确定的，所以可以分离出来放在main中确定大小
                         //但是MYTBF_MAX是job数组的大小，必须要在模块编译时确定，所以这个宏被放在了mytbf.h中

typedef void mytbf_t;    //数据结构应该被隐藏在mytbf.c中
//如果不用void，那么在.h文件中又要申明一次mytbf_st结构体，否则编译会找不到mytbf_st这个类型，所以比较麻烦，用void一了百了

//对外接口只有4个   其他都由内部实现
mytbf_t * mytbf_init(int cps, int burst);  //用void来隐藏数据类型，实际上返回的是mytbf_st的指针

int mytbf_fetchtocken(mytbf_t * tbf, int size);

int mytbf_returntoken(mytbf_t * tbf, int size);  //不丢掉token，废物利用，这样不浪费权限

int mytbf_destroy(mytbf_t * tbf);


#endif