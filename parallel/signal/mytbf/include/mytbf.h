#ifndef MYTBFS_H__
#define MYTBFS_H__

#define MYTBF_MAX  1024
typedef void mytbf_t;


mytbf_t * mytbf_init(int cps, int burst);  //用void来隐藏数据类型，实际上返回的是mytbf_st的指针

int mytbf_fetchtocken(mytbf_t * tbf, int token);

int mytbf_returntoken(mytbf_t * tbf, int token);

int mytbf_destroy(mytbf_t * tbf);


#endif