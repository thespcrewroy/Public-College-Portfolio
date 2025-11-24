#include <stdio.h> // for printf, fprintf, stderr
#include <string.h> // for strlen, strncmp, bzero
#include <time.h> // for time functions
#include <stdbool.h> // for bool type
#include "fs.h" // for Inode, SuperBlock, Dentry, TYPE, MAX_DIR_ENTRY, SMALL_FILE, MAGIC_NUMBER
#include "disk.h" // for disk_mount, disk_umount, disk_read, disk_write, BLOCK_SIZE

/*
* Compare two command strings for equality.
*
* @param comm: first command string
* @param comm2: second command string
* @return: true if equal, false otherwise
*/
bool command(char *comm, char *comm2)
{
	if(strlen(comm) == strlen(comm2) && strncmp(comm, comm2, strlen(comm)) == 0) return true; // compare two commands for equality
	return false; // return false if not equal
} // command

/*
* Main function to run the file system simulator.
*
* @param argc: number of command line arguments
* @param argv: array of command line argument strings
* @return: 0 on success, -1 on failure
*/
int main(int argc, char **argv)
{
	char input[64+16+16+16+SMALL_FILE]; // input buffer
	char comm[64], arg1[16], arg2[16], arg3[16], arg4[SMALL_FILE]; // command and arguments

	srand(0); // seed random number generator

	if(argc < 2) { // if not enough arguments
		fprintf(stderr, "usage: ./fs disk_name\n"); // print usage message
		return -1; // return error code
	} // if
	srand(0); // seed random number generator
		
	printf("sizeof inode: %d, sizeof superblock: %d, sizeof Dentry (Dir Entry): %d\n", sizeof(Inode), sizeof(SuperBlock), sizeof(Dentry)); // print sizes of structures
	fs_mount(argv[1]); // mount file system
	printf("%% "); // print prompt
	while(fgets(input, 256, stdin)) // read input from user
	{
		bzero(comm,64); bzero(arg1,16); bzero(arg2,16); bzero(arg3,16); bzero(arg4, SMALL_FILE); // clear command and argument buffers
		int numArg = sscanf(input, "%s %s %s %s %s", comm, arg1, arg2, arg3, arg4); // parse input into command and arguments
		if(command(comm, "quit")) break; // if command is 'quit', exit loop
		else if(command(comm, "exit")) break; // if command is 'exit', exit loop
		else execute_command(comm, arg1, arg2, arg3, arg4, numArg - 1); // execute command with arguments

		printf("%% "); // print prompt
	} // while

	fs_umount(argv[1]); // unmount file system
} // main

