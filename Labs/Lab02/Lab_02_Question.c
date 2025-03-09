#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 20

/* Global array of natural numbers */
int list[SIZE] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    11, 12, 13, 14, 15, 16, 17, 18, 19, 20
};

/* Structure to pass parameters (sublist indices) to each thread */
typedef struct {
    int from_index;
    int to_index;
} parameters;

/* Thread function: computes the sum of the elements in list[from_index ... to_index] */
void *runner(void *param) {
    parameters *data = (parameters *) param;
    int i;
    
    /* Allocate memory for the partial sum */
    int *partial_sum = malloc(sizeof(int));
    if (partial_sum == NULL) {
        perror("Failed to allocate memory");
        pthread_exit(NULL);
    }
    *partial_sum = 0;

    /* Sum the designated sublist */
    for (i = data->from_index; i <= data->to_index; i++) {
        *partial_sum += list[i];
    }

    /* Free the parameters structure as it is no longer needed */
    free(data);

    /* Exit the thread and return the partial sum */
    pthread_exit(partial_sum);
}

int main() {
    pthread_t tid[2];          // Two thread IDs
    pthread_attr_t attr;       // Thread attributes structure
    int total_sum = 0;
    int i;
    int *partial_sum;

    /* Initialize thread attributes to default values */
    pthread_attr_init(&attr);

    /* Create first thread to sum the first half of the list */
    parameters *data1 = malloc(sizeof(parameters));
    if (data1 == NULL) {
        perror("Failed to allocate memory");
        exit(1);
    }
    data1->from_index = 0;
    data1->to_index = (SIZE / 2) - 1;  // For SIZE=20, indices 0 to 9
    if (pthread_create(&tid[0], &attr, runner, data1) != 0) {
        perror("Failed to create thread 1");
        exit(1);
    }

    /* Create second thread to sum the second half of the list */
    parameters *data2 = malloc(sizeof(parameters));
    if (data2 == NULL) {
        perror("Failed to allocate memory");
        exit(1);
    }
    data2->from_index = SIZE / 2;      // For SIZE=20, index 10
    data2->to_index = SIZE - 1;          // indices 10 to 19
    if (pthread_create(&tid[1], &attr, runner, data2) != 0) {
        perror("Failed to create thread 2");
        exit(1);
    }

    /* Wait for both threads to finish and collect their results */
    for (i = 0; i < 2; i++) {
        if (pthread_join(tid[i], (void **)&partial_sum) != 0) {
            perror("Failed to join thread");
            exit(1);
        }
        total_sum += *partial_sum;
        free(partial_sum);  // Free the memory allocated in the thread
    }

    /* Output the final sum */
    printf("Sum of numbers in the list is: %d\n", total_sum);

    return 0;
}