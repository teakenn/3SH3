// Completed code for the assignment 2, to run execute the following
// gcc -pthread A2.c -o A2
// ./A2 <desired simultion time as integer> if no simulation time specified, program will run for 30 seconds



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define NUM_CHAIRS 3
#define NUM_STUDENTS 10

// Global flag for controlling the simulation
volatile int simulationRunning = 1;

// Shared variables for waitlist queue
int waiting = 0;                 // number of students waiting
int queue[NUM_CHAIRS];           // FIFO queue for student IDs
int front = 0, rear = 0, count = 0;  // Indices and count for the queue

// Mutex to protect shared data (waiting count + queue)
pthread_mutex_t mutex;

// Semaphores
sem_t sem_students;              // Counts how many students are waiting
sem_t sem_student[NUM_STUDENTS]; // One semaphore per student (for keeping waitlist order)


// -Queue logic- //
void enqueue(int studentID) {
    // Caller must hold mutex
    queue[rear] = studentID;
    rear = (rear + 1) % NUM_CHAIRS;
    count++;
}

int dequeue() {
    // Caller must hold mutex
    int id = queue[front];
    front = (front + 1) % NUM_CHAIRS;
    count--;
    return id;
}

// -TA thread- //
void *ta_function(void *arg) {
    while (1) {
        // Wait until at least one student is waiting (or until unblocked when ending)
        sem_wait(&sem_students);

        // Check if simulation ended and no one is waiting
        pthread_mutex_lock(&mutex);
        if (!simulationRunning && waiting == 0) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        if (waiting > 0) {
            // Dequeue the oldest waiting student
            int next_student_id = dequeue();
            waiting--;
            printf("TA: Calling student %d. Waiting count: %d\n",
                   next_student_id, waiting);

            pthread_mutex_unlock(&mutex);

            // Signal exactly that student
            sem_post(&sem_student[next_student_id]);

            // Simulate helping the student
            int help_time = rand() % 4 + 1;
            printf("TA: Helping student %d for %d seconds.\n",
                   next_student_id, help_time);
            sleep(help_time);
        } else {
            pthread_mutex_unlock(&mutex);
        }
    }
    printf("TA: Office hours over. TA thread exiting.\n");
    pthread_exit(NULL);
}

// -Student thread- //
void *student_function(void *arg) {
    int id = *((int *)arg);
    free(arg);

    while (simulationRunning) {
        // Simulate programming
        int prog_time = rand() % 10 + 2;
        printf("Student %d: Programming for %d seconds.\n", id, prog_time);
        sleep(prog_time);

        if (!simulationRunning) break;

        // Try to get help
        printf("Student %d: Needs help.\n", id);
        pthread_mutex_lock(&mutex);
        if (waiting < NUM_CHAIRS) {
            enqueue(id);
            waiting++;
            printf("Student %d: Seated in queue. Waiting count: %d\n", id, waiting);

            sem_post(&sem_students);
            pthread_mutex_unlock(&mutex);

            sem_wait(&sem_student[id]);

            if (!simulationRunning) break;

            printf("Student %d: Getting help now.\n", id);
        } else {
            pthread_mutex_unlock(&mutex);
            printf("Student %d: No available chairs, will try again.\n", id);
        }
    }
    printf("Student %d: Simulation ended. Exiting thread.\n", id);
    pthread_exit(NULL);
}

// -main()- //
int main(int argc, char *argv[]) {
    srand(time(NULL));

    // Default simulation time if not provided via command-line.
    int simulationTime = 30; //default time

    // Check if the user provided a simulation time as the first argument.
    if (argc > 1) {
        simulationTime = atoi(argv[1]);
        if (simulationTime <= 0) {
            simulationTime == 30;
        }
    }

    printf("Simulation time set to %d seconds!\n", simulationTime);

    pthread_t ta;
    pthread_t students[NUM_STUDENTS];

    // Initialize mutex and semaphores
    pthread_mutex_init(&mutex, NULL);
    sem_init(&sem_students, 0, 0);
    for (int i = 0; i < NUM_STUDENTS; i++) {
        sem_init(&sem_student[i], 0, 0);
    }

    // Create TA thread
    pthread_create(&ta, NULL, ta_function, NULL);

    // Create student threads
    for (int i = 0; i < NUM_STUDENTS; i++) {
        int *id = malloc(sizeof(int));
        if (id == NULL) {
            perror("Failed to allocate memory for student ID");
            exit(EXIT_FAILURE);
        }
        *id = i;  // Student IDs: 0 to NUM_STUDENTS-1
        pthread_create(&students[i], NULL, student_function, id);
    }

    // Run the simulation for the specified time.
    sleep(simulationTime);
    printf("MAIN: Office hours are over! Ending the simulation...\n");
    simulationRunning = 0;

    // Unblock TA and any blocked student threads.
    sem_post(&sem_students);
    for (int i = 0; i < NUM_STUDENTS; i++) {
        sem_post(&sem_student[i]);
    }

    pthread_join(ta, NULL);
    for (int i = 0; i < NUM_STUDENTS; i++) {
        pthread_join(students[i], NULL);
    }

    // Cleanup resources.
    pthread_mutex_destroy(&mutex);
    sem_destroy(&sem_students);
    for (int i = 0; i < NUM_STUDENTS; i++) {
        sem_destroy(&sem_student[i]);
    }

    printf("MAIN: Simulation ended.\n");
    return 0;
}
