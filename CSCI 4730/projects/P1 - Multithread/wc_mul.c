/*************************************************
 * C program to count no of lines, words and 	 *
 * characters in a file.		                 *
 *************************************************/

#include <stdio.h>	// library functions for file operations
#include <stdlib.h>	// atoi, exit, abort, malloc/free
#include <sys/types.h>	// pid_t
#include <sys/wait.h>	// wait, waitpid, WIFEXITED, WEXITSTATUS, WIFSIGNALED, WTERMSIG
#include <unistd.h>	// fork, pipe, read, write, close, _exit, getpid (POSIX syscalls)
#include <string.h>	// memset, memcpy

#define MAX_PROC 100 // maximum number of processes
#define MAX_FORK 1000 // maximum number of forks

typedef struct count_t { // struct to hold line, word, and character counts
	int linecount; // initialize an integer 'linecount' to track number of lines
	int wordcount; // initialize an integer 'wordcount' to track number of words
	int charcount; // initialize an integer 'charcount' to track number of characters
} count_t; // count_t

typedef struct plist_t {
	pid_t pid; // child process id
	long offset; // file offset where the child starts reading
	long size; // number of bytes the child should read
	int pipefd[2]; // pipe file descriptors: pipefd[0] for reading, pipefd[1] for writing
} plist_t; // plist_t


int CRASH = 0; // percentage chance of child process crashing


count_t word_count(FILE* fp, long offset, long size)
{
	// Declare instance variables
	count_t count = {0,0,0}; // initialize a struct 'count_t' variable 'count'
	long rbytes = 0; // initialize a long 'rbytes' to track the number of bytes read
	int ch; // initialize a character 'ch' to track each character in the file

	// Initialize counter variables
	count.linecount = 0;
	count.wordcount = 0;
	count.charcount = 0;

	// Read file
	printf("[pid %d] reading %ld bytes from offset %ld\n", getpid(), size, offset);
	if (fseek(fp, offset, SEEK_SET) < 0) { // if setting the 'read' pointer to start of fiile (includes offset) fails
			printf("[pid %d] fseek error!\n", getpid()); // display error message
			return count; // return count with all 0s
	} // if

	while ((ch=getc(fp)) != EOF && rbytes < size) { // read each character till end of file or size limit
		if (ch != ' ' && ch != '\n') { ++count.charcount; } // increment character count if NOT new line or space
		if (ch == ' ' || ch == '\n') { ++count.wordcount; } // increment word count if new line or space character
		if (ch == '\n') { ++count.linecount; } // increment line count if new line character
		rbytes++; // track the number of bytes read
	} // while

	
	srand(getpid()); // seed the random number generator with the process ID
	if(CRASH > 0 && (rand()%100 < CRASH)) // if CRASH rate is set and random number is less than CRASH rate
	{
		printf("[pid %d] crashed.\n", getpid()); // display crash message
		abort(); // abort the process
	} // if
	 

	return count; // return the thread 'count' with the line, word, and character counts
} // word_count

int main(int argc, char **argv)
{

	// Declare instance variables
	long fsize; // initialize a long 'fsize' to track the size of the file
	FILE *fp; // initialize a file pointer 'fp'
	int numJobs; // initialize an integer 'numJobs' to track the number of child processes

	// Process list
	plist_t plist[MAX_PROC];
	count_t total, count;
	int i, pid, status;
	int nFork = 0;

	// Error check command line arguments
	if(argc < 3) { // if there are less than 3 arguments
		printf("usage: wc_mul <# of processes> <filname>\n"); // display usage message
		return 0; // exit the program
	} // if

	
	if(argc > 3) {
		CRASH = atoi(argv[3]);
		if(CRASH < 0) CRASH = 0;
		if(CRASH > 50) CRASH = 50;
	}
	printf("CRASH RATE: %d\n", CRASH);
	

	// Number of Child Processes
	numJobs = atoi(argv[1]); // convert command line argument 1 to integer 'numJobs'
	if (numJobs < 1) numJobs = 1; // if 'numJobs' is less than 1, set it to 1
	if(numJobs > MAX_PROC) numJobs = MAX_PROC; // if 'numJobs' is greater than MAX_PROC, set it to MAX_PROC

	// Initialize total counts
	total.linecount = 0;
	total.wordcount = 0;
	total.charcount = 0;

	// Open file
    fp = fopen(argv[2], "r"); // open the file in command line argument 2 for reading
	if(fp == NULL) { // if there was an error opening the file
		printf("File open error: %s\n", argv[2]); // display error message
		printf("usage: wc <# of processes> <filname>\n"); // display usage message
		return 0; // exit the program
	} // if

	// Determine file size
	fseek(fp, 0L, SEEK_END); // set 'read' pointer to the end of the file (0 offset)
	fsize = ftell(fp); // get the current 'read' pointer position which is the size of the file
    if (fsize < 0) { // if ftell failed
        printf("fseek end failed"); // display error message
        fclose(fp); // close the file
        return 1; // exit the program
    } // if
    fclose(fp); // close the file

	// Calculate file offset and size to read for each child process
	long base = fsize / numJobs; // base size each child should read
    long rem  = fsize % numJobs; // remainder bytes to be distributed among the first 'rem' children
    long cur  = 0; // current offset in the file

	// Code Idea from Instructor: offset and size calculation
	for(i = 0; i < numJobs; i++) { // for each child process
		// Set Pipe
		plist[i].offset = cur; // set the file offset for this child process
		plist[i].size = base; // set the base size for this child process
		if(rem > 0) { // if there are remainder bytes to distribute
			plist[i].size++; // increment the size for this child process by 1
			rem--; // decrement the remainder count
		} // if
		cur += plist[i].size; // update the current offset for the next child process
		plist[i].pid = -1; // initialize the child PID to -1 (useful for crash testing in Part B)
		plist[i].pipefd[0] = plist[i].pipefd[1] = -1; // initialize the pipe file descriptors to -1 (useful for crash testing in Part B)

		if (pipe(plist[i].pipefd) < 0) { // create a pipe for this child process
			printf("Pipe creation error.\n"); // if pipe creation failed, display error message
			return 1; // exit the program
		} // if

		if (i + 1 > MAX_FORK) { // if the number of forks exceeds MAX_FORK
			printf("Too many forks.\n"); // display error message
			return 1; // exit the program
		} // if

		pid = fork(); // fork a new process
		if(pid < 0) { // fork failed
			printf("Fork failed.\n"); // display error message
			return 1; // exit the program
		} else if(pid == 0) { // child process
			close(plist[i].pipefd[0]); // close the read end of the pipe in the child process

			FILE* childFp; // file pointer for the child process
			
			// Word Count
			childFp = fopen(argv[2], "r"); // open the file in command line argument 2 for reading

			if(childFp == NULL) { // if there was an error opening the file
				// Write zeroes so the parent does not hang
				count_t zero = {0, 0, 0}; // initialize zero counts
				(void)write(plist[i].pipefd[1], &zero, sizeof(zero)); // write zero counts to the pipe
				close(plist[i].pipefd[1]); // child closes its write end
				exit(0); // end child cleanly (don’t run parent code)
			} // if

			count_t child_count = word_count(childFp, plist[i].offset, plist[i].size); // call 'word_count' for this child process
			fclose(childFp); // close the file in the child process

			// Send results to parent
			ssize_t wbytes = write(plist[i].pipefd[1], &child_count, sizeof(child_count)); // write the count results to the pipe
			if (wbytes != sizeof(child_count)) { // if the number of bytes written is
				printf("[pid %d] write to pipe error!\n", getpid()); // display error message
				close(plist[i].pipefd[1]); // close the write end of the pipe
				exit(1); // exit the child process
			} // if

			close(plist[i].pipefd[1]);   // child done writing
			exit(0);                    // exit child process
		} else { // parent process
			plist[i].pid = pid; // store the child PID in the process list
			close(plist[i].pipefd[1]); // close the write end of the pipe in the parent process
			nFork++; // increment the number of forks
		} // if
	}

	// Parent
	// wait for all children
	// check their exit status
	// read the result of normalliy terminated child
	// re-crete new child if there is one or more failed child
	// close pipe

	int finished = 0; // number of finished child processes

	while (finished < numJobs) { // while the number of finished child processes is less than the total number of jobs
		int status; // variable to hold the status of the child process
		pid_t cpid = waitpid(-1, &status, 0); // wait for any child process to finish
		if (cpid < 0) { perror("waitpid"); exit(1); } // if waitpid failed, display error message and exit

		// Find which child this is
		int childIndex = -1; // index of the child process in the process list
		for (int k = 0; k < numJobs; k++) { // loop through the process list
			if (plist[k].pid == cpid) { childIndex = k; break; } // if the PID matches, store the index and break
		} // for
		if (childIndex < 0) { // if the index is not found
			continue; // continue to the next iteration of the loop
		} // if

		if (WIFEXITED(status) && WEXITSTATUS(status) == 0) { // if the child process exited normally
			count_t c = (count_t){0,0,0}; // initialize a count_t struct to hold the counts from the child
			ssize_t r = read(plist[childIndex].pipefd[0], &c, sizeof(c)); // read the counts from the pipe
			if (r == sizeof(c)) { // if the number of bytes read is correct
				total.linecount += c.linecount; // add the counts to the total counts
				total.wordcount += c.wordcount; // add the counts to the total counts
				total.charcount += c.charcount; // add the counts to the total counts
			} // if
			close(plist[childIndex].pipefd[0]); // close the read end of the pipe
			plist[childIndex].pid = -1; // mark this slot as available
			finished++; // increment the number of finished child processes
		} else { // if the child process did not exit normally
			  if (WIFSIGNALED(status)) {
				fprintf(stderr, "Child %d (slot %d) killed by signal %d — respawning\n",
						(int)cpid, childIndex, WTERMSIG(status));
			} else {
				fprintf(stderr, "Child %d (slot %d) exited abnormally (%d) — respawning\n",
						(int)cpid, childIndex, WEXITSTATUS(status));
			} // els
			// Restart the child process
			close(plist[childIndex].pipefd[0]); // close the read end of the pipe
			if (pipe(plist[childIndex].pipefd) < 0) { perror("pipe"); exit(1); } // create a new pipe for the child process

			pid_t npid = fork(); // fork a new process
			if (npid < 0) { perror("fork"); exit(1); } // if fork failed, display error message and exit

			if (npid == 0) { // if this is the child process
				close(plist[childIndex].pipefd[0]); // child writes only
				FILE *childFp = fopen(argv[2], "r"); // open the file in command line argument 2 for reading
				if (!childFp) { // if there was an error opening the file
					count_t zero = (count_t){0,0,0}; // initialize zero counts
					(void)write(plist[childIndex].pipefd[1], &zero, sizeof(zero)); // write zero counts to the pipe
					close(plist[childIndex].pipefd[1]); // close the write end of the pipe
					exit(0); // exit the child process
				} // if
				count_t child_count = word_count(childFp, plist[childIndex].offset, plist[childIndex].size); // call 'word_count' for this child process
				fclose(childFp); // close the file in the child process
				ssize_t w = write(plist[childIndex].pipefd[1], &child_count, sizeof(child_count)); // write the count results to the pipe
				if (w != sizeof(child_count)) { // if the number of bytes written is incorrect
					close(plist[childIndex].pipefd[1]); // close the write end of the pipe
					_exit(1); // exit the child process
				} // if
				close(plist[childIndex].pipefd[1]);  // child done writing
				_exit(0); // exit the child process
			} else { // if this is the parent process
				plist[childIndex].pid = npid; // store the new child PID in the process list
				close(plist[childIndex].pipefd[1]);  // parent reads only
			} // else
		} // else
	} // while


	printf("\n========== Final Results ================\n");
	printf("Total Lines : %d \n", total.linecount); // display total line count
	printf("Total Words : %d \n", total.wordcount); // display total word count
	printf("Total Characters : %d \n", total.charcount); // display total character count
	printf("=========================================\n");

	return(0); // exit the program
} // main

