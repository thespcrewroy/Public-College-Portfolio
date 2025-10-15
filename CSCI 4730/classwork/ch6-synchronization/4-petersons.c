#include <stdio.h> // for printf
#include <pthread.h> // for pthreads

int flag[2]; // create two flags to track which of the two threads wants to enter critical section
int turn; // whose turn is it?
int count = 0; // shared counter

/* 
 * This lock function implements Peterson's algorithm to ensure mutual exclusion.
 */
void lock_init() {
    flag[0] = flag[1] = 0; // initially, no thread wants to enter critical section
    turn = 0; // initially, it's thread 0's turn
} // lock_init

/*
* This lock function indicates that a thread wants to enter the critical section.
* It uses Peterson's algorithm to ensure mutual exclusion.
*
* @param self: the ID of the calling thread (0 or 1)
*/
void lock(int self) {
    flag[self] = 1; // indicate that this thread wants to enter critical section
    turn = 1 - self; // give turn to the other thread

    while (flag[1 - self] == 1 && turn == 1 - self) { // while the other thread wants to enter and it's their turn
        // busy wait
    } // while
} // lock

/*
 * This unlock function indicates that a thread is leaving the critical section.
 *
 * @param self: the ID of the calling thread (0 or 1)
 */
void unlock(int self) {
    flag[self] = 0; // indicate that this thread is leaving critical section
} // unlock

/*
* This function performs a simple task of incrementing a counter for a thread.
*
* @param arg: a pointer to the argument passed to the thread (used to identify the thread)
*/
void* func(void *s) {
    int i = 0; // loop variable
    int self = (int *)s; // get thread ID from argument
    printf("Thread Entered: %d\n", self); // print thread ID

    for (i = 0; i < 1000000; i++) { // do a lot of work
        lock(self); // enter critical section
        count++; // increment the shared counter
        unlock(self); // leave critical section
    } // for
} // func

/*
* This program demonstrates the creation and joining of two threads using Peterson's algorithm for synchronization.
* Each thread increments a shared counter a million times, ensuring mutual exclusion to avoid race conditions. It * satisfies progress by ensuring that if one thread is not interested, the other can proceed. It satisfies ounded * waiting by limiting the number of times a thread can be bypassed.
*/
int main() {
    pthread_t p1, p2; // initialize thread variables
    lock_init(); // initialize the lock

    pthread_create(&p1, NULL, func, (void *)0); // create thread 0
    pthread_create(&p2, NULL, func, (void *)1); // create thread 1

    pthread_join(p1, NULL); // wait for thread 0 to finish
    pthread_join(p2, NULL); // wait for thread 1 to finish

    printf("Final Count: %d\n", count); // print the final value of the shared counter
    return 0; // exit the program
} // main