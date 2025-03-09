#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int amnt = 0;               //global variable inititation

// protect shared variable
pthread_mutex_t lock;

// deposit func
void *deposit(void *param) {

  int deposit_amnt = *((int *) param);        //initiaye integer pointer, obtain deposit amount

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

  pthread_exit(NULL);

}


// withdrawal func
void *withdraw(void *param) {

  int withdraw_amnt = *((int *) param);

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
  
  pthread_exit(NULL);
  
}

int main(int argc, char *argv[]) {

  //convert to integers
  int dep_amnt = atoi(argv[1]);
  int with_amnt = atoi(argv[2]);
  
  // init. mutex w/ defaults
  if (pthread_mutex_init(&lock, NULL) != 0) {
    perror("Mutex initialization failed");
    exit(EXIT_FAILURE);
  }
  
  // create thread identifiers
  pthread_t threads[6];
  int ret;
  int i;
  
  //create threads for depositing money
  for (i = 0; i < 3; i++) {
    ret = pthread_create(&threads[i], NULL, deposit, &dep_amnt);
    if (ret != 0) {
      perror("Error creating deposit thread");
      exit(EXIT_FAILURE);
      
    }
  }
  
  //create withdraw threads
  for (i = 3; i <6; i++) {
    ret = pthread_create(&threads[i], NULL, withdraw, &with_amnt);
    if (ret != 0) {
      perror("Error creating withdraw thread");
      exit(EXIT_FAILURE);
    
    }
  }
  
  //Let threads complete
  for (i = 0; i < 6; i++) {
    ret = pthread_join(threads[i], NULL);
    if (ret != 0) {
      perror("Error joining thread");
      exit(EXIT_FAILURE);
    }
  }
  
  
  //print final balance
  printf("Final balance: %d\n", amnt);
  
  
  pthread_mutex_destroy(&lock);
  
  return 0;
  
}
  
  
