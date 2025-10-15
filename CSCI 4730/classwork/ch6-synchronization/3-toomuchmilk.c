#include <stdio.h> // for printf
#include <pthread.h> // for pthreads
#include <stdatomic.h> // for atomics

/* Global Variables */
int counter = 0; // counts the number of times mythread is run
static _Atomic int noteA = 0; // flag to indicate thread A's intention to enter critical section
static _Atomic int noteB = 0; // flag to indicate thread B's intention to enter critical section

/*
* This function performs a simple task of incrementing a counter for a thread.
* 
* @param arg: a pointer to the argument passed to the thread (used to identify the thread)
* @return: NULL when the thread exits
*/
void *mythread(void *arg) {
    char id = *(char*)arg; // determine thread identity 'A' or 'B'
    int i = 0; // loop variable

    while (i < 10000) { // do a lot of work
        if (id == 'A') { // if this is thread A
            atomic_store(&noteA, 1); // notify intent to enter critical section
            while (atomic_load(&noteB)) { // wait while thread B is interested
                /* spin / do nothing */
            } // wait
            counter++; // critical section: increment the counter
            atomic_store(&noteA, 0); // notify exit from critical section
            i++; // work done; count this iteration
        } else { // if this is thread B
            atomic_store(&noteB, 1); // notify intent to enter critical section
            if (!atomic_load(&noteA)) { // if thread A is not interested
                counter++; // critical section: increment the counter
                atomic_store(&noteB, 0); // notify exit from critical section
                i++; // work done; count this iteration
            } else { // else, if thread A is interested
                atomic_store(&noteB, 0); // back off and retry later
            } // else
        } // else
    } // for
    return NULL; // exit the thread
} // mythread

/*
* This program demonstrates the creation and joining of two threads.
* Now two threads are incrementing the same counter simultaneously, which can result in a race condition.
* Synchronization is necessary when multiple threads access shared data to prevent lost updates
*            
* Thread A: leave note_A, wait while note_B, do work, remove note_A
* Thread B: leave note_B, if !note_A do work and remove note_B, else remove note_B and retry later
*/
int main(void) {
    pthread_t p1, p2; // initialize thread variables
    char A = 'A', B = 'B'; // thread identifiers

    printf("main: begin (counter = %d)\n", counter); // print the initial global counter value

    pthread_create(&p1, NULL, mythread, &A); // create thread A
    pthread_create(&p2, NULL, mythread, &B); // create thread B
    pthread_join(p1, NULL); // wait for thread A to finish
    pthread_join(p2, NULL); // wait for thread B to finish

    printf("main: done (counter = %d)\n", counter); // print the final global counter value
    return 0; // exit the program
} // main