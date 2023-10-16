#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <sys/wait.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <fcntl.h> 
#include <errno.h> 
#include <sys/stat.h> 
#include <pthread.h>   // for pthreads 
#include <time.h>  // for clock_gettime

int isNumeric(const char*);

typedef struct _thread_data_t { 
    int localTid; 
    const int *data; 
    int numVals; 
    pthread_mutex_t *lock; 
    long long int *totalSum; 
} thread_data_t;

int main( int argc, char* argv[]) {
    if(argc != 2) {
        printf("There aren't enough parameters\n");
        return -1;
    }

    if(isNumeric(argv[1]) == 0) { // implies non numeric argument at second position (index position 1)
        printf("Inappropriate element in the position of thread count\n");
        return -1;
    }

    printf("Welcome\n");
}

int isNumeric(const char *str) {
    char *endptr; // Used by strtol
    strtol(str, &endptr, 10);
    return (*endptr == '\0' && *str != '\0') ? 1 : 0;
}