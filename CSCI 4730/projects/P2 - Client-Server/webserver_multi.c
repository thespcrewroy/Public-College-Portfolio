#include <stdio.h> // standard I/O functions
#include <stdlib.h> // standard library functions
#include <unistd.h> // POSIX API functions
#include <arpa/inet.h> // definitions for internet operations
#include <pthread.h> // POSIX threads
#include <string.h> // string manipulation functions
#include "webserver.h" // custom header file for webserver
#include <semaphore.h> // semaphore functions

#define MAX_REQUEST 100 // shared buffer capacity

//----------------------------------------------------------------------------------------------
// INSTANCE VARIABLES
//----------------------------------------------------------------------------------------------

int port; // server port number command line argument
int numThread; // number of worker threads comand line argument
pthread_t listenerThread; // listener thread
pthread_t workerThreads[MAX_REQUEST]; // array of worker threads


/* Shared Bounded Buffer For Incoming Requests */
int requestBuffer[MAX_REQUEST]; // shared buffer for incoming requests
int produceIndex = 0; // next write index in the buffer
int consumeIndex = 0; // next read index in the buffer

/* Synchronization - Bounded Buffer Problem */
sem_t sem_empty; // semaphore to track empty slots in the buffer
sem_t sem_full; // semaphore to track full slots in the buffer
pthread_mutex_t mutex; // mutex to protect access to the buffer


//----------------------------------------------------------------------------------------------
// FUNCTIONS
//----------------------------------------------------------------------------------------------

/**
 * Producer Thread
 * Enqueues incoming client fds into the shared buffer
 * 
 * @param client: client socket file descriptor
 */
void producer(int client) {
	/* Wait For Empty Slot */
	sem_wait(&sem_empty); // wait for an empty slot in the buffer
	pthread_mutex_lock(&mutex); // lock the mutex to protect buffer access

	/* Add Client To Buffer*/
	requestBuffer[produceIndex] = client; // add client fd to the buffer at produceIndex
	produceIndex = (produceIndex + 1) % MAX_REQUEST; // update produceIndex in a circular manner

	/* Signal Full Buffer */
	pthread_mutex_unlock(&mutex); // unlock the mutex
	sem_post(&sem_full); // signal that there is a new full slot in the buffer
} // producer

/**
 * Creates the TCP socket
 * Accepts connections in a loop
 * Drops new client fds into the shared buffer
 * Similar to listener() in the single-thread version, except that it enqueues to the buffer instead
 * 
 * @param arg: void pointer (needed for pthreads, not used here)
 */
void *listener(void *arg) {
	/* Declare and Initialize Variables */
	(void)arg; // suppress unused parameter warning
	int r; // return value for socket functions
	struct sockaddr_in sin; // server address structure
	struct sockaddr_in peer; // client address structure
	int peer_len = sizeof(peer); // length of client address structure
	int sock; // server socket file descriptor
	int options = 1; // socket options

	/* Create TCP socket */
	sock = socket(AF_INET, SOCK_STREAM, 0); // create TCP socket
	if (sock < 0) { // if socket creation fails
        perror("socket failed"); // print error message
        pthread_exit(NULL); // exit thread
    } // if
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &options, sizeof(options)); // allows for quick reuse of the port

	/* Initialize Server Address Structure */
	memset(&sin, 0, sizeof(sin)); // zero out the server address structure
	sin.sin_family = AF_INET; // IPv4
	sin.sin_addr.s_addr = INADDR_ANY; // bind to any address
	sin.sin_port = htons(port); // set port number


	/* Bind Socket */
	r = bind(sock, (struct sockaddr *) &sin, sizeof(sin)); // bind socket to address
	if(r < 0) { // if bind fails
		perror("binding failed:"); // print error message
		close(sock); // close the socket
		pthread_exit(NULL); // exit thread
		return; // exit function
	} // if

	/* Start Listening */
	r = listen(sock, 5); // listen for incoming connections
	if(r < 0) { // if listen fails
		perror("Error listening socket:"); // print error message
		close(sock); // close the socket
		pthread_exit(NULL); // exit thread
		return; // exit function
	} // if

	printf("HTTP server listening on port %d\n", port);

	/* Producer Loop */
	while (1)
	{
		/* Accept Incoming Connections */
		int s = accept(sock, NULL, NULL); // accept incoming connection
		if (s < 0) { // if accept fails
			perror("accept failed"); // print error message
			break; // exit loop
		} // if

		producer(s); // enqueue the client socket fd into the shared buffer
	} // while

	close(sock); // close the server socket
	pthread_exit(NULL); // exit thread
} // listener

/**
 * Consumer Function
 * Dequeues a client fd from the shared buffer
 * 
 * @return client socket file descriptor
 */
int consumer() {
	int client = requestBuffer[consumeIndex]; // get client fd from buffer at consumeIndex
    consumeIndex = (consumeIndex + 1) % MAX_REQUEST; // update consumeIndex in a circular manner
    return client; // return the client fd
} // consumer

/**
 * Worker Thread
 * Continuously dequeues client fds from the shared buffer and processes requests.
 * 
 * @param arg: void pointer (needed for pthreads, not used here)
 */
void *worker(void *arg) {
	(void)arg; // suppress unused parameter warning

	while (1) { // infinite loop
		/* Wait For Full Buffer */
        sem_wait(&sem_full); // do not consume from an empty buffer 
        pthread_mutex_lock(&mutex); // lock buffer, remove one fd
        int client = consumer(); // get client fd from buffer
        pthread_mutex_unlock(&mutex); // unlock buffer
        sem_post(&sem_empty); // signal that there is a new empty slot in the buffer

        /* Process Client Request */
        process(client);  // from net.c: process the client request
        close(client); // close the client socket
    }
    return NULL; // needs to return a value due to pthreads
} // worker

/**
 * Initialize the semaphores and mutex
 * Create 1 listener and N worker threads
 * Join the threads (blocking in thread pool forever, for this is a simple server)
 */
void thread_control() {
    // Initialize Syncrhonization Parameters
    pthread_mutex_init(&mutex, NULL); // initialize mutex for buffer access
    sem_init(&sem_empty, 0, MAX_REQUEST); // initialize empty semaphore to MAX_REQUEST
    sem_init(&sem_full, 0, 0); // initialize full semaphore to 0

	// Error Handling
	if (numThread > MAX_REQUEST) { // if number of threads exceeds MAX_REQUEST
        numThread = MAX_REQUEST; // cap number of threads at MAX_REQUEST
    } // if
	if (pthread_create(&listenerThread, NULL, listener, NULL) != 0) { // if thread creation fails
        perror("pthread_create(listener)"); // print error message
        exit(1); // exit program
    } // if
    // create worker threads (consumers)
    for (int i = 0; i < numThread; i++) { // for each worker thread
        if (pthread_create(&workerThreads[i], NULL, worker, NULL) != 0) { // if thread creation fails
            perror("pthread_create(worker)"); // print error message
            exit(1); // exit program
        } // if
    } // for

	/* Thread Join*/
    pthread_join(listenerThread, NULL); // join the listener thread
    for (int i = 0; i < numThread; i++) { // if listener ever exits, 
        pthread_join(workerThreads[i], NULL); // join worker threads
    } // for

    /* Cleanup - Not Usually Reached */
    pthread_mutex_destroy(&mutex); // destroy mutex
    sem_destroy(&sem_empty); // destroy empty semaphore
    sem_destroy(&sem_full); // destroy full semaphore
} // thread_control

int main(int argc, char *argv[])
{
	if(argc != 3 || atoi(argv[1]) < 2000 || atoi(argv[1]) > 50000) { 
		fprintf(stderr, "./webserver_multi PORT(2001 ~ 49999) #_of_threads\n"); // print usage message
		return 0; // exit program
	} // if

	int i; // loop variable
	port = atoi(argv[1]); // get port number from command line argument
	numThread = atoi(argv[2]); // get number of threads from command line argument
	thread_control(); // start thread control (listener and worker threads)
	return 0; // exit program
} // main
