#include <stdio.h> // for printf
#include <pthread.h> // for pthreads

int counter = 0; // shared counter
pthread_mutex_t lock; // mutex lock for synchronizing access to the counter

/*
 * This function performs a simple task of incrementing a counter for a thread.
 *
 * @param arg: a pointer to the argument passed to the thread (not used here)
 * @return: NULL when the thread exits
 */
void *mythread(void *arg) {
    int i;

    for (i = 0; i < 10000; i++) { // do a lot of work
        pthread_mutex_lock(&lock); // acquire the lock before entering critical section
        counter++; // increment the shared counter
        pthread_mutex_unlock(&lock); // release the lock after leaving critical section
    } // for

    return NULL; // exit the thread
} // mythread

/*
 * This program demonstrates the creation and joining of two threads using a mutex lock for synchronization.
 * Each thread increments a shared counter a thousand times, ensuring mutual exclusion to prevent data races.
 */
int main(int argc, char * argv[]) {
    pthread_t p1, p2; // initialize thread variables

    if (pthread_mutex_init(&lock, NULL) != 0) { // initialize the mutex lock
        printf("mutex init failed\n"); // print error message if initialization fails
        return 1; // exit if mutex initialization fails
    } // if

    printf("main: begin (counter = %d)\n", counter); // print the initial global counter value
    pthread_create(&p1, NULL, mythread, "A"); // create a new thread that runs mythread with argument "A"
    pthread_create(&p2, NULL, mythread, "B"); // create a new thread that runs mythread with argument "B"
    
    // Join Waits for the Threads to Finish
    pthread_join(p1, NULL); // wait for thread p1 to finish
    pthread_join(p2, NULL); // wait for thread p2 to finish
    printf("main: done (counter = %d)\n", counter); // print the final global counter value
    pthread_mutex_destroy(&lock); // destroy the mutex lock
    return 0; // exit the program
} // main