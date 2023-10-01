#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define ARRAY_SIZE 1024
#define NUM_THREADS 4

typedef struct {
    int start;
    int thread_id;
    int end;
    int* array;
    double* partial_sum;
} ThreadData;

void* calculate_partial_average(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    double partial_sum = 0.0;

    for (int i = data->start; i < data->end; i++) {
        partial_sum += data->array[i];
        printf("Calculating in thread %d\n", data->thread_id);
    }

    *(data->partial_sum) = partial_sum;

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];
    int array[ARRAY_SIZE];
    double partial_sums[NUM_THREADS] = {0};

    // Initialize the array with random values
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = rand() % 100; // Fill the array with random values between 0 and 99
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].start = i * (ARRAY_SIZE / NUM_THREADS);
        thread_data[i].end = (i == NUM_THREADS - 1) ? ARRAY_SIZE : (i + 1) * (ARRAY_SIZE / NUM_THREADS);
        thread_data[i].array = array;
        thread_data[i].partial_sum = &partial_sums[i];
        thread_data[i].thread_id = i + 1;

        pthread_create(&threads[i], NULL, calculate_partial_average, &thread_data[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    double total_sum = 0.0;
    for (int i = 0; i < NUM_THREADS; i++) {
        total_sum += partial_sums[i];
    }

    double average = total_sum / ARRAY_SIZE;
    printf("Average: %lf\n", average);

    return 0;
}
