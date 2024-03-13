#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>

int main(){

    int64_t count = 0;
    time_t end;
    end = time(NULL) + 5;
    
    while(time(NULL) < end){
        count++;
    }
    printf("%ld\n", count);
 
    exit(0);

}