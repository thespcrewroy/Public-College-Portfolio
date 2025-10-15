#include <stdio.h> // for printf
#include <string.h> // for memset
#include <stdlib.h> // for malloc, free, rand
#include <pthread.h> // for pthreads
#include <unistd.h> // for sleep
#include <semaphore.h> // for semaphores

/* Gloval Variables */
int n_items_in_buffer = 0; // number of items in the buffer
pthread_mutex_t lock; // mutex lock for synchronizing access to the buffer
sem_t sem_empty; // semaphore for empty slots in the buffer
sem_t sem_full; // semaphore for full slots in the buffer
int MAX_SLOT = 5; // maximum number of slots in the buffer
int NUM_CONSUMER = 3; // number of consumer threads

/*
 * Function to be executed by the producer thread. It produces items and adds them to the buffer.
 */
void *producer() {
    while(1) { // infinite loop
    printf("[Producer] produce an item\n"); // print message

    /* ----------- ADDITIONS -----------*/
    sem_wait(&sem_empty); // wait for an empty slot
    pthread_mutex_lock(&lock); // acquire lock
    /* ----------- ADDITIONS -----------*/

    n_items_in_buffer++; // increment number of items in buffer
    printf("[Producer] added the item to the buffer. # items in buffer = %d\n", n_items_in_buffer); // print number of items in buffer
    usleep(rand() % 500000); // sleep for a random time to represent procesing overhead

    /* ----------- ADDITIONS -----------*/
    pthread_mutex_unlock(&lock); // release lock
    sem_post(&sem_full); // signal that a new item is added
    /* ----------- ADDITIONS -----------*/
    } // while
} // producer

/*
 * Function to be executed by the consumer threads. It removes items from the buffer.
 */
void *consumer(void *arg) {
    printf("[Consumer %d] joined\n", syscall(186) - getpid()); // print message with thread id
    while(1) { // infinite loop
        /* ----------- ADDITIONS -----------*/
        sem_wait(&sem_full); // wait for a full slot
        pthread_mutex_lock(&lock); // acquire lock
        /* ----------- ADDITIONS -----------*/

        n_items_in_buffer--; // decrement number of items in buffer
        printf("[Consumer %d] removed an item from the buffer. # items in buffer = %d\n", syscall(186) - getpid(), n_items_in_buffer); // print number of items in buffer
        usleep(rand() % 800000); // sleep for a random time to represent procesing overhead

        /* ----------- ADDITIONS -----------*/
        pthread_mutex_unlock(&lock); // release lock
        sem_post(&sem_empty); // signal that an item is removed
        /* ----------- ADDITIONS -----------*/
    } // while
} // consumer

/*
 * This program demonstrates a bounded buffer problem using producer and consumer threads.
 * The producer adds items to the buffer, and the consumers remove items from the buffer.
 * Synchronization is achieved using mutex locks and semaphores to ensure that the buffer does not
 * exceed its maximum capacity and that consumers do not attempt to remove items from an empty buffer.
 */
int main(int argc, char *argv[]) {
    int i; // loop variable

    pthread_t p; // producer thread
    pthread_t c[10]; // array of consumer threads

    /* ----------- ADDITIONS -----------*/
    pthread_mutex_init(&lock, NULL); // initialize mutex
    sem_init(&sem_empty, 0, MAX_SLOT); // initialize semaphore for empty slots
    sem_init(&sem_full, 0, 0); // initialize semaphore for full slots
    /* ----------- ADDITIONS -----------*/

    for (i = 0; i < NUM_CONSUMER; i++) { // loop to create consumer threads
        pthread_create(&c[i], NULL, consumer, NULL); // create consumer threads and execute consumer function
    } // for
    pthread_create(&p, NULL, producer, NULL); // create producer thread and execute producer function

    for (i = 0; i < NUM_CONSUMER; i++) { // loop to join consumer threads
        pthread_join(c[i], NULL); // join consumer threads
    } // for
    pthread_join(p, NULL); // join producer thread

    return 0; // return success
} // main