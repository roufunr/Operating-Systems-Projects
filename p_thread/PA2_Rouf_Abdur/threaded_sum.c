#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <sys/wait.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <fcntl.h> 
#include <errno.h> 
#include <sys/stat.h> 
#include <sys/time.h>
#include <pthread.h> // for thread

typedef struct _thread_data_t { 
    const int *data; 
    int startInd; 
    int endInd; 
    pthread_mutex_t *lock; 
    long long int *totalSum; 
} thread_data_t;

int readFile(char[], int[]);
void * arraySum(void *);
void extra_load();
void * arraySumEXTRA(void *);

int main(int argc, char* argv[]) {
    if(argc != 3) {
        printf("There aren't enough parameters!\n");
        return -1;
    }
    
    // get command line input
    char* data_filename = argv[1];
    char* number_of_thread_in_str = argv[2];
    int number_of_thread = atoi(number_of_thread_in_str);
    
    int capacity = 1000 * 1000 * 1000; // assuming this is the highest size of the dynamic array
    
    // read all data from file and set to dynamic array
    int * data = (int *)malloc(capacity * sizeof(int));
    int data_size = readFile(data_filename, data);
    if(data_size == -1) {
        return -1;
    }

    if(data_size < number_of_thread) {
        printf("Too many threads requested\n");
        return -1;
    }

    printf("data size: %d\n", data_size);
    
    struct timeval start_time, end_time;
    // Get the current time
    if (gettimeofday(&start_time, NULL) == -1) {
        perror("gettimeofday");
        return -1;
    }
    long long int total_sum;
    total_sum = 0;
    pthread_t threads[number_of_thread];
    thread_data_t thread_data[number_of_thread];
    pthread_mutex_t sumMutex;
    if (pthread_mutex_init(&sumMutex, NULL) != 0) {
        perror("Mutex initialization failed");
        return -1;
    }
    
    // chunk 1 to chunk N - 1
    for(int i = 0; i < number_of_thread - 1; i++) {
        thread_data[i].data = data;
        thread_data[i].startInd = i * (data_size / number_of_thread); // inclusive
        thread_data[i].endInd = (i + 1) * (data_size / number_of_thread); // exclusive
        thread_data[i].totalSum = &total_sum;
        thread_data[i].lock = &sumMutex;
        pthread_create(&threads[i], NULL, arraySum, &thread_data[i]);
        //pthread_create(&threads[i], NULL, arraySumEXTRA, &thread_data[i]);
    }
    // chunk N
    thread_data[number_of_thread - 1].data = data;
    thread_data[number_of_thread - 1].startInd = (number_of_thread - 1) * (data_size / number_of_thread); // inclusive
    thread_data[number_of_thread - 1].endInd = data_size; // exclusive
    thread_data[number_of_thread - 1].totalSum = &total_sum;
    thread_data[number_of_thread - 1].lock = &sumMutex;
    pthread_create(&threads[number_of_thread - 1], NULL, arraySum, &thread_data[number_of_thread - 1]);

    // join all thread with main thread
    for(int i = 0; i < number_of_thread; i++) {
        pthread_join(threads[i], NULL);
    }

    // Get the current time
    if (gettimeofday(&end_time, NULL) == -1) {
        perror("gettimeofday");
        return -1;
    }
    // printf("Star time: %ld\n", start_time.tv_usec);
    // printf("End time: %ld\n", end_time.tv_usec);
    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000.0 + (end_time.tv_usec - start_time.tv_usec) / 1000.0;
    printf("Total number of threads: %d\n", number_of_thread);
    printf("Total sum: %lld\n", total_sum);
    printf("Elapsed time: %lf ms\n", elapsed_time);
    pthread_exit(NULL);
}

void extra_load() {
    long long total_sum = 0;
    for (long long i = 0; i < 5000LL; i++) {
        total_sum += i;
    }
}

void * arraySum(void * arg) {
    thread_data_t *thread_data = (thread_data_t *)arg;
    long long int threadSum = 0;
    for(int i = thread_data->startInd; i < thread_data->endInd; i++) {
        threadSum += thread_data->data[i];
        extra_load(); // calling a cpu intensive function
    }
    pthread_mutex_lock(thread_data->lock);
    *(thread_data->totalSum) += threadSum; //critical section
    pthread_mutex_unlock(thread_data->lock);
    pthread_exit(NULL);
}

void * arraySumEXTRA(void * arg) {
    thread_data_t *thread_data = (thread_data_t *)arg;
    for(int i = thread_data->startInd; i < thread_data->endInd; i++) {
        pthread_mutex_lock(thread_data->lock);
        //critical section
        *(thread_data->totalSum) += thread_data->data[i];
        // extra_load(); // calling a cpu intensive function
        pthread_mutex_unlock(thread_data->lock);
        
    }
    pthread_exit(NULL);
}

int readFile(char filename[], int data[]) {
    FILE *datafile;
    datafile = fopen(filename, "r");
    if(datafile == NULL) {
        printf("File not found...\n");
        return -1;
    }
    char buffer[200];
    int idx = 0;
    while (fscanf(datafile, "%s", buffer) != EOF) {
        int number = atoi(buffer);
        data[idx++] = number;
    }

    int number_of_values = idx;
    return number_of_values; // successfully read the whole file
}