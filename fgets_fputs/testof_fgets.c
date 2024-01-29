#include <stdio.h>
#include <stdlib.h>

#define SIZE 5
int main(int argc, char *argv[])
{
    char buf[SIZE];
    while(fgets(buf, SIZE, stdin) != NULL)
            fputs(buf, stdout);

    exit(EXIT_SUCCESS);
}