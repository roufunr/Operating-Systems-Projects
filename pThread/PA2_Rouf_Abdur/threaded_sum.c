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
#include <pthread.h>

int readFile(char[], int[]);
int getDataSize(char *);

typedef struct _thread_data_t { 
    const int *data; 
    int startInd; 
    int endInd; 
    pthread_mutex_t *lock; 
    long long int *totalSum; 
} thread_data_t;

int main(int argc, char* argv[]) {
    if(argc != 3) {
        printf("There aren't enough parameters!\n");
        return -1;
    }
    
    // get command line input
    char* data_filename = argv[1];
    char* number_of_thread_in_str = argv[2];
    int number_of_thread = atoi(number_of_thread_in_str);
    
    // get the data capacity from filename
    int capacity = getDataSize(data_filename);
    if (capacity == 0) {
        printf("There is an issue of naming the data file!\n");
        return -1;
    }
    
    // read all data from file and set to dynamic array
    int * data = (int *)malloc(capacity * sizeof(int));
    if(readFile(data_filename, data) == -1) {
        return -1;
    }
    

}

int getDataSize(char * data_filename) {
    int capacity;
    if(!strcmp("oneThousand.txt", data_filename)) 
    {
        capacity = 1000;
    } 
    else if (!strcmp("oneMillion.txt", data_filename))
    {
        capacity = 1000 * 1000;
    } 
    else if (!strcmp("oneHundredThousand.txt", data_filename)) // for the sake of covering more input cases
    {
        capacity = 100 * 1000;
    } 
    else if (!strcmp("fiveHundredThousand.txt", data_filename)) // for the sake of covering more input cases
    {
        capacity = 5 * 100 * 1000;
    } 
    else 
    {
        capacity = 0; // edge case
    }
    return capacity;
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
    int capacity = 1000;
    while (fscanf(datafile, "%s", buffer) != EOF) {
        int number = atoi(buffer);
        data[idx++] = number;
    }
    return 0; // successfully read the whole file
}