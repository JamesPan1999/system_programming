#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    int count = 0;  //记录文件中字符数

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    while(fgetc(fp) != EOF)
    {
        count++;
    }
    fclose(fp);
    fprintf(stdout, "The file  has %d characters.\n", count);
}