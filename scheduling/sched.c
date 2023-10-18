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
#include "progress.h"

int isNumeric(const char*);
void * arraySum(void *);

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
        printf("Inappropriate element type in the position of thread count\n");
        return -1;
    }

    int number_of_threads = atoi(argv[1]);
    int capacity = 2 * 1000 * 1000; // assuming this is the highest size of the dynamic array
    
    
    // read all data from file and set to dynamic array
    int * data = (int *)malloc(capacity * sizeof(int));
    
    // assign random values to dynamic array
    srand(time(NULL));
    for(int i = 0; i < capacity; i++) {
        int random_number = rand();
        data[i] = random_number;
    }
    long long int totalSum = 0;
    pthread_mutex_t mutex_lock;
    if (pthread_mutex_init(&mutex_lock, NULL) != 0) {
        perror("Mutex initialization failed");
        return -1;
    }
    
    thread_data_t thread_data[number_of_threads];
    for(int i = 0; i < number_of_threads; i++) {
        thread_data[i].localTid = i;
        thread_data[i].data = data;
        thread_data[i].numVals = capacity;
        thread_data[i].totalSum = &totalSum;
        thread_data[i].lock = &mutex_lock;
    }
    
    pthread_t threads[number_of_threads];
    for(int i = 0; i < number_of_threads; i++) {
        pthread_create(&threads[i], NULL, arraySum, &thread_data[i]);
    }

    // join all thread with main thread
    for(int i = 0; i < number_of_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex_lock);
    free(data);
    return 0;
}

void * arraySum(void * arg) {
    thread_data_t *thread_data = (thread_data_t *)arg;
    long long int maximum_latency = 0;
    while(1) {
         struct timespec start, end;

        clock_gettime(CLOCK_REALTIME, &start);
        long long int threadSum = 0;
        for(int i = 0; i < thread_data->numVals; i++) {
            threadSum += thread_data->data[i];
        }
        pthread_mutex_lock(thread_data->lock);
        *(thread_data->totalSum) += threadSum; //critical section
        pthread_mutex_unlock(thread_data->lock);

        clock_gettime(CLOCK_REALTIME, &end);
        long long int latency = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);

        maximum_latency = latency > maximum_latency ? latency : maximum_latency;
        print_progress(thread_data->localTid, maximum_latency);
    }
    pthread_exit(NULL);
}

int isNumeric(const char *str) {
    char *endptr; // Used by strtol
    strtol(str, &endptr, 10);
    return (*endptr == '\0' && *str != '\0') ? 1 : 0;
}