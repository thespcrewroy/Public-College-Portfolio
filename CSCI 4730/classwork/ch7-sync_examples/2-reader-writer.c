#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

/* Global Variables */
sem_t rw_mutex; // controls access to shared data
sem_t mutex; // controls access to read_count
int max_writer = 1; // only one writer at a time
int max_reader = 13; // up to 13 readers at a time
long count = 1; // shared data
int read_count = 0; // number of readers reading

void *writer(void *s) {
    int self = (int *)s; // obtain thread ID

    /* ----------- ADDITIONS -----------*/
    sem_wait(&rw_mutex); // wait for rw_mutex
    /* ----------- ADDITIONS -----------*/

    count = count * 10; // modify critical shared data
    printf("Writer %d modified count to %ld\n", self, count); // print modified data

    /* ----------- ADDITIONS -----------*/
    sem_post(&rw_mutex); // signal rw_mutex
    /* ----------- ADDITIONS -----------*/
} // writer

void *reader(void * s) {
    int self = (int *)s; // obtain thread ID

    /* ----------- ADDITIONS -----------*/
    sem_wait(&mutex); // wait for mutex
    read_count++; // increment reader count
    if (read_count == 1) sem_wait(&rw_mutex); // wait for rw_mutex if first reader
    sem_post(&mutex); // signal mutex
    /* ----------- ADDITIONS -----------*/

    printf("Reader %d: count as %ld\n", self, count); // print critical shared data

    /* ----------- ADDITIONS -----------*/
    sem_wait(&mutex); // wait for mutex
    read_count--; // decrement reader count
    if (read_count == 0) sem_post(&rw_mutex); // signal rw_mutex if last reader
    sem_post(&mutex); // signal mutex
} // reader

int main() {
    long i = 0; // loop variable
    pthread_t read[max_reader]; // read threads
    pthread_t write[max_reader]; // write thread

    /* ----------- ADDITIONS -----------*/
    sem_init(&rw_mutex, 0, 1); // initialize rw_mutex to 1
    sem_init(&mutex, 0, 1); // initialize mutex to 1
    /* ----------- ADDITIONS -----------*/

    for (i = 0; i < max_writer; i++) { // create writer threads
        pthread_create(&write[i], NULL, (void *)writer, (void *)i); // create writer threads
    } // for
    for (i = 0; i < max_reader; i++) { // create reader threads
        pthread_create(&read[i], NULL, (void *)reader, (void *)i); // create reader threads
    } // for

    for (i = 0; i < max_reader; i++) { // wait for reader threads to finish
        pthread_join(read[i], NULL); // wait for reader threads to finish
    } // for
    for (i = 0; i < max_writer; i++) { // wait for writer threads to finish
        pthread_join(write[i], NULL); // wait for writer threads to finish
    } // for

    /* ----------- ADDITIONS -----------*/
    sem_destroy(&rw_mutex); // destroy rw_mutex
    sem_destroy(&mutex); // destroy mutex
    /* ----------- ADDITIONS -----------*/

    return 0; // exit main thread
} // main