///Run using ./<executable name> amnt

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

int amnt = 0;               //global variable inititation

// protect shared variable
pthread_mutex_t lock;

//semaphores to control deposits/withdrawals
sem_t deposit_sem;
sem_t withdraw_sem;

// deposit func
void *deposit(void *param) {

  int deposit_amnt = *((int *) param);        //initiaye integer pointer, obtain deposit amount

  //Wait on deposit_sem, makes sure amount is below specified
  if (sem_wait(&deposit_sem) != 0) {
    perror("Deposit failed. Please ensure account balance is below specified amount.");
    pthread_exit(NULL);
  }


  if (pthread_mutex_lock(&lock) != 0) {
    perror("Failed to lock mutex in deposit");
    pthread_exit(NULL);
  }


  amnt += deposit_amnt;
  printf("Deposited %d, New balance: %d\n", deposit_amnt, amnt);


  if (pthread_mutex_unlock(&lock) != 0) { 
    perror("Failed to unlock mutex in deposit");
    pthread_exit(NULL);
  }

  // Signal withdraw thread
  if(sem_post(&withdraw_sem) != 0) {
    perror("sem_post failed in deposit");
    pthread_exit(NULL);
  }

  pthread_exit(NULL);

}


// withdrawal func
void *withdraw(void *param) {

  int withdraw_amnt = *((int *) param);

  //Wait on withdraw_sem, makes sure amount is below specified
  if (sem_wait(&withdraw_sem) != 0) {
    perror("Withdraw failed. Please ensure account balance is below specified amount.");
    pthread_exit(NULL);
  }

  if (pthread_mutex_lock(&lock) != 0) {
    perror("Failed to lock mutex in withdraw :(");
    pthread_exit(NULL);
  }

  amnt -= withdraw_amnt;
  printf("Withdrew %d, New balance: %d\n", withdraw_amnt, amnt);

  if (pthread_mutex_unlock(&lock) != 0) {
    perror("Failed to unlock mutex in withdraw");
    pthread_exit(NULL);
  }

  // Signal deposit thread
  if(sem_post(&deposit_sem) != 0) {
    perror("sem_post failed in withdraw");
    pthread_exit(NULL);
  }
  
  pthread_exit(NULL);
  
}

int main(int argc, char *argv[]) {

  //ensure correct command line argument is provided
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <transaction_amnt>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  int trans_amnt = atoi(argv[1]);
  if (trans_amnt != 100) {
    fprintf(stderr, "Error: transaction amount must be 100.\n");
    exit(EXIT_FAILURE);
  }
  
  // init. mutex w/ defaults
  if (pthread_mutex_init(&lock, NULL) != 0) {
    perror("Mutex initialization failed");
    exit(EXIT_FAILURE);
  }
  
  if (sem_init(&deposit_sem, 0, 4) != 0) {
    perror("Failed to initialize deposit semaphore");
    exit(EXIT_FAILURE);
  }

  if (sem_init(&withdraw_sem, 0, 0) != 0) {
    perror("Failed to initialize withdraw semaphore");
    exit(EXIT_FAILURE);
  }


  pthread_t threads[10];
  int ret, i;
    
  // Create 7 deposit threads
  for (i = 0; i < 7; i++) {
    ret = pthread_create(&threads[i], NULL, deposit, (void *)&trans_amnt);
    if (ret != 0) {
      perror("Error creating deposit thread");
      exit(EXIT_FAILURE);
    }
  }
    
  // Create 3 withdrawal threads
  for (i = 7; i < 10; i++) {
    ret = pthread_create(&threads[i], NULL, withdraw, (void *)&trans_amnt);
    if (ret != 0) {
      perror("Error creating withdraw thread");
      exit(EXIT_FAILURE);
    }
  }
    
  // Wait for all threads to complete
  for (i = 0; i < 10; i++) {
    ret = pthread_join(threads[i], NULL);
    if (ret != 0) {
      perror("Error joining thread");
      exit(EXIT_FAILURE);
    }
  }
    
  printf("Final balance: %d\n", amnt);
    
  // Clean up, destroy the mutex and semaphores
  pthread_mutex_destroy(&lock);
  sem_destroy(&deposit_sem);
  sem_destroy(&withdraw_sem);
    
  return 0;
}