#include <stdio.h> // for standard I/O functions
#include <stdlib.h> // for rand()
#include <time.h> // for time functions
#include "fs.h" // for file system structures and constants

//----------------------------------------------------------------------------------------------
// FUNCTIONS
//----------------------------------------------------------------------------------------------

/*
* Generates a random string of given size to fill the file with on file_create(char * name, int size) in fs.c.
*
* @param str: pointer to the string buffer
* @param size: size of the string to generate
* @return: size of the generated string
*/
int rand_string(char *str, size_t size)
{
	if(size < 1) return 0; // if size is less than 1, return 0
	int n, key; // loop counter and random key
	const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"; // character set to choose from
	for (n = 0; n < size; n++) { // loop to generate each character
		key = rand() % (int) (sizeof charset - 1); // get random index
		str[n] = charset[key]; // assign character to string
	} // for
	str[size] = '\0'; // null-terminate the string
	return size+1; // return size of the generated string
} // rand_string

/*
* Bit manipulation functions for inodeMap and blockMap
*
* @param array: pointer to the bitmap array
* @param index: index of the bit to manipulate
*/
void toggle_bit(char *array, int index)
{
	array[index/8] ^= 1 << (index % 8); // toggle the bit at the given index
} // toggle_bit

/*
* Get the value of a bit in the bitmap
*
* @param array: pointer to the bitmap array
* @param index: index of the bit to get
* @return: value of the bit (0 or 1)
*/
char get_bit(char *array, int index)
{
	return 1 & (array[index/8] >> (index % 8)); // get the value of the bit at the given index
} // get_bit

/*
* Set the value of a bit in the bitmap. 
* Will be used to clean up inode bitmap and data bitmap on file deletion.
*
* @param array: pointer to the bitmap array
* @param index: index of the bit to set
* @param value: value to set the bit to (0 or 1)
*/
void set_bit(char *array, int index, char value)
{
	if(value != 0 && value != 1) return; // if invalid value, return
	array[index/8] ^= 1 << (index % 8); // toggle the bit
	if(get_bit(array, index) == value) return; // if the bit is already set to the desired value, return
	toggle_bit(array, index); // toggle the bit again to set it to the desired value
} // set_bit

/*
* Find next free and available i-node from the inodeMap.
* Set the bit in the inodeMap.
* Decrease freeInodeCount in superBlock.
*
* @return: index of the free inode (inode number), -1 if none available
*/
int get_free_inode()
{
	int i = 0; // loop counter
	for(i = 0; i < MAX_INODE; i++) // loop through all inodes
	{
		if(get_bit(inodeMap, i) == 0) { // if the inode is free
			set_bit(inodeMap, i, 1); // set the bit in the inodeMap
			superBlock.freeInodeCount--; // decrease freeInodeCount in superBlock
			return i; // return the inode number
		} // if
	} // for

	return -1; // return -1 if no free inode found
} // get_free_inode

/*
* Find next free and available data block from the blockMap.
* Set the bit in the blockMap.
* Decrease freeBlockCount in superBlock.
*
* @return: index of the free block (block number), -1 if none available
*/
int get_free_block()
{
	int i = 0; // loop counter
	for(i = 0; i < MAX_BLOCK; i++) // for each block used by the file
	{
		if (get_bit(blockMap, i) == 0) { // if the block is free
			set_bit(blockMap, i, 1); // set the bit in the blockMap
			superBlock.freeBlockCount--; // decrease freeBlockCount in superBlock
			return i; // return the block number
		} // if
	} // for

	return -1; // return -1 if no free block found
} // get_free_block

/*
* Format timeval structure into a human-readable string.
*
* @param tv: pointer to the timeval structure
* @param buf: buffer to store the formatted string
* @param sz: size of the buffer
* @return: number of characters written, or -1 on error
*/
int format_timeval(struct timeval *tv, char *buf, size_t sz)
{
	ssize_t written = -1; // number of characters written
	struct tm *gm; // pointer to tm structure
	gm = gmtime(&tv->tv_sec); // convert time to GMT

	if (gm) // if conversion is successful
	{
		written = (ssize_t)strftime(buf, sz, "%Y-%m-%d %H:%M:%S", gm); // format time
		if ((written > 0) && ((size_t)written < sz)) // if there is space in the buffer
		{
			int w = snprintf(buf+written, sz-(size_t)written, ".%06dZ", tv->tv_usec); // append microseconds
			written = (w > 0) ? written + w : -1; // update number of characters written
		} // if
	} // if
	return written; // return number of characters written
} // format_timeval
