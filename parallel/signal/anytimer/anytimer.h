#ifndef __ANYTIMER_H__
#define __ANYTIMER_H__

typedef void at_jobfunc_t(void *);

int at_addjob(int sec, at_jobfunc_t *jobp, void *arg);  //第二个输入函数句柄，第三个输入函数对应的参数，这里实际是char，用void隐藏了
/*
 * 模拟了文件描述符的思路！  
 * return >= 0  成功，返回任务ID
 *        == -EINVAL 失败，参数非法
 *        == -ENOSPC 失败，任务队列满了
 *        == -ENOMEM 失败，内存不够
 */

int at_canclejob(int id);
/*
 * 取消任务
 * return == 0  成功，指定任务已成功
 *        == -EINVAL 失败，参数非法
 *        == -ENOENT 失败，任务不存在
 *        == -EBUSY 失败，任务已经完成
 *        == -ECANCE 失败，任务重复取消
 */

int at_waitjob(int id);  //类似wait
/*
 * 等待任务
 * return == 0  成功，指定任务已释放
 *        == -EINVAL 失败，参数非法
 */

#endif