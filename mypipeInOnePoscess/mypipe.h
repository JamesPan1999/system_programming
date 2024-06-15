#pragma once
#define PIPESIZE 1024

typedef void mypipe_t;

mypipe_t *mypipe_init(void);

int mypipe_read(mypipe_t *p, char *buf, size_t count);
int mypipe_write(mypipe_t *p, const char *buf, size_t count);

int mypipe_destroy(mypipe_t *p);
