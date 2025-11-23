#include <stdio.h> // for FILE, fopen, fread, fwrite, fclose, printf, fprintf, stderr
#include <string.h> // for memcpy
#include "disk.h" // for MAX_BLOCK, BLOCK_SIZE


//----------------------------------------------------------------------------------------------
// INSTANCE VARIABLES
//----------------------------------------------------------------------------------------------

char disk[MAX_BLOCK][BLOCK_SIZE]; // simulated disk as a 2D array of bytes

//----------------------------------------------------------------------------------------------
// FUNCTIONS
//----------------------------------------------------------------------------------------------

/*
* Read a block from the simulated disk into the buffer.
* Define a new variable with malloc for buf before calling this function
*
* @param block: block number to read
* @param buf: buffer to store the read data
* @return: 0 on success, -1 on error
*/
int disk_read(int block, char *buf)
{
	if(block < 0 || block >= MAX_BLOCK) { // if invalid block number
		printf("disk_read error\n"); // invalid block number
		return -1; // return error code
	} // if
	memcpy(buf, disk[block], BLOCK_SIZE); // copy the block data to the buffer

	return 0; // return success code
} // disk_read

/*
* Write a block from the buffer to the simulated disk.
* Use memcpy to copy the data from buf to your desired location.
*
* @param block: block number to write
* @param buf: buffer containing the data to write
* @return: 0 on success, -1 on error
*/
int disk_write(int block, char *buf)
{
	if(block < 0 || block >= MAX_BLOCK) { // if invalid block number
		printf("disk_write error\n"); // invalid block number
		return -1; // return error code
	} // if
	memcpy(disk[block], buf, BLOCK_SIZE); // copy the buffer data to the block

	return 0; // return success code
} // disk_write

/*
* Mount the simulated disk from a file.
*
* @param name: name of the file to mount the disk from
* @return: 1 on success, 0 on error
*/
int disk_mount(char *name)
{
	FILE *fp = fopen(name, "r"); // open the file for reading
	if(fp != NULL) { // if file opened successfully
		fread(disk, BLOCK_SIZE, MAX_BLOCK, fp); // read the disk data from the file
		fclose(fp); // close the file
		return 1; // return success code
	} // if
	return 0; // return error code
} // disk_mount

/*
* Unmount the simulated disk to a file.
*
* @param name: name of the file to unmount the disk to
* @return: 1 on success, -1 on error
*/
int disk_umount(char *name)
{
	FILE *fp = fopen(name, "w"); // open the file for writing
	if(fp == NULL) { // if file open failed
		fprintf(stderr, "disk_umount: file open error! %s\n", name); // print error message
		return -1; // return error code
	} // if

	fwrite(disk, BLOCK_SIZE, MAX_BLOCK, fp); // write the disk data to the file
	fclose(fp); // close the file
	return 1; // return success code
} // disk_umount

