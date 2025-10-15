#include <stdio.h> // for printf
#include <stddef.h> // for NULL (optional if you already include stdio.h)
#include <pthread.h> // for pthreads

/* Global Variables */
int counter = 0; // counts the number of times mythread is run

/*
 * This function performs a simple task of incrementing a counter for a thread.
 * 
 * @param arg: a pointer to the argument passed to the thread (not used here)
 * @return: NULL when the thread exits
 */
void *mythread(void *arg) {

  int i; // loop variable
  for (i = 0; i < 10000; i++) { // do a lot of work
    counter++; // increment the counter
  } // for

  return NULL; // exit the thread
} // mythread

/*
 * This program demonstrates demonstrates the creation and joining of two threads.
 * Now two threads are incrementing the same counter simultaneously, which can result in a race condition.
 * Synchronization is necessary when multiple threads access shared data to prevent lost updates
 */
int main(int argc, char * argv[]) {
    pthread_t p1,p2; // initialize thread variables
    printf("main: begin (counter = %d)\n", counter); // print the initial global counter value

    
    pthread_create(&p1, NULL, mythread, "A"); // create a new thread that runs mythread with argument "A"
    pthread_create(&p2, NULL, mythread, "B"); // create a new thread that runs mythread with argument "B"
    pthread_join(p1, NULL); // join waits for the threads to finish
    pthread_join(p2, NULL); // join waits for the threads to finish

    printf("main: done (counter = %d)\n", counter); // print the final global counter value
    return 0; // exit the program
} // main