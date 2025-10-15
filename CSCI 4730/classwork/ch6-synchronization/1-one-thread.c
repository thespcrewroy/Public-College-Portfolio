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
 * This program demonstrates the creation and joining of a single thread.
 * It starts with a global variable counter set to 0, creates one thread 
 * that runs a loop incrementing counter 10,000 times, and then main() waits, u
 * sing pthread_join, for the thread to finish, then prints the result.
 */
int main(int argc, char * argv[]) {
    pthread_t p1; // initialize a thread variable
    printf("main: begin (counter = %d)\n", counter); // print the initial global counter value
    pthread_create(&p1, NULL, mythread, "A"); // create a new thread that runs mythread with argument "A"
    pthread_join(p1, NULL); // join waits for the threads to finish
    printf("main: done (counter = %d)\n", counter); // print the final global counter value
    return 0; // exit the program
} // main