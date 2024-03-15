 int getitimer(int which, struct itimerval *curr_value);
       int setitimer(int which, const struct itimerval *new_value,
                     struct itimerval *old_value);

  struct itimerval {
               struct timeval it_interval; /* Interval for periodic timer */
               struct timeval it_value;    /* Time until next expiration */
           };

           struct timeval {
               time_t      tv_sec;         /* seconds */
               suseconds_t tv_usec;        /* microseconds */
           };

其中，it_interval类似于单片机中定时器的倒计时寄存器，it_value是当前倒数的时间计>数值。 每次计完一个周期后，it_interval将会赋值给it_value，此操作是原子的
此外，setitimer没有累计时间误差，特别适用于服务器程序
