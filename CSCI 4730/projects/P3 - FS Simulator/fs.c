#include <stdio.h> // for FILE, fopen, fread, fwrite, fclose, printf, fprintf, stderr
#include <string.h> // for strncpy, memcpy
#include <stdlib.h> // for malloc, free, atoi
#include "fs.h" // for Inode, SuperBlock, Dentry, TYPE, MAX_DIR_ENTRY, SMALL_FILE, MAGIC_NUMBER
#include "fs_util.h" // for set_bit, format_timeval, rand_string, command
#include "disk.h" // for disk_mount, disk_umount, disk_read, disk_write, BLOCK_SIZE

//----------------------------------------------------------------------------------------------
// INSTANCE VARIABLES
//----------------------------------------------------------------------------------------------

char inodeMap[MAX_INODE / 8]; // inode bitmap
char blockMap[MAX_BLOCK / 8]; // block bitmap
Inode inode[MAX_INODE]; // inode table
SuperBlock superBlock; // superblock
Dentry curDir; // current directory
int curDirBlock; // current directory block number


//----------------------------------------------------------------------------------------------
// FUNCTIONS
//----------------------------------------------------------------------------------------------

/*
* Mount the file system from a disk file.
*
* @param name: name of the disk file
* @return: 0 on success
*/
int fs_mount(char *name)
{
		int numInodeBlock =  (sizeof(Inode)*MAX_INODE)/ BLOCK_SIZE; // number of inode blocks in the disk
		int i, index, inode_index = 0; // index for inode array


		// Load superblock, inodeMap, blockMap and inodes into Memory
		if (disk_mount(name) == 1) { // if disk mount success
				disk_read(0, (char*) &superBlock); // read superblock
				if (superBlock.magicNumber != MAGIC_NUMBER) { // if invalid magic number
						printf("Invalid disk!\n"); // print error message 'invalid disk'
						exit(0); // exit program
				} // if
				disk_read(1, inodeMap); // read inodeMap
				disk_read(2, blockMap); // read blockMap
				for (i = 0; i < numInodeBlock; i++) // read inode blocks
				{
						index = i+3; // inode blocks start from block 3
						disk_read(index, (char*) (inode+inode_index)); // read inode block into inode array
						inode_index += (BLOCK_SIZE / sizeof(Inode)); // move to next set of inodes
				} // for
				// Root Directory
				curDirBlock = inode[0].directBlock[0]; // get root directory block number
				disk_read(curDirBlock, (char*)&curDir); // read root directory into curDir
		} else { // if disk mount failed, initialize file system
				// Initialize the File System Superblock from fs.h
				superBlock.magicNumber = MAGIC_NUMBER; // set magic number
				superBlock.freeBlockCount = MAX_BLOCK - (1+1+1+numInodeBlock); // number of free blocks are total blocks - (superblock(1) + inodeMap(1) + blockMap(1) + inode blocks)
				superBlock.freeInodeCount = MAX_INODE; // number of free inodes is MAX_INODE

				// Initialize inodeMap from fs.h
				for (i = 0; i < MAX_INODE / 8; i++) // for each byte in inodeMap
				{
						set_bit(inodeMap, i, 0); // set all bits to 0 (free)
				} // for

				// Initialize blockMap from fs.h
				for (i = 0; i < MAX_BLOCK / 8; i++) // for each byte in blockMap
				{
						if (i < (1+1+1+numInodeBlock)) set_bit(blockMap, i, 1); // set used blocks (superblock, inodeMap, blockMap, inode blocks) to 1 (used)
						else set_bit(blockMap, i, 0); // set free blocks to 0 (free)
				} // for

				// Initialize Root Directory
				int rootInode = get_free_inode(); // get inode number for root directory
				curDirBlock = get_free_block(); // get block number for root directory

				// Create a New I-node Entry from fs.h
				inode[rootInode].type = directory; // set type to directory
				inode[rootInode].owner = 0; // set owner to 0 for directory
				inode[rootInode].group = 0; // set group to 0 for directory
				gettimeofday(&(inode[rootInode].created), NULL); // set creation time
				gettimeofday(&(inode[rootInode].lastAccess), NULL); // set last access time
				inode[rootInode].size = 1; // set size to 1 byte
				inode[rootInode].blockCount = 1; // set block count to 1
				inode[rootInode].directBlock[0] = curDirBlock; // set direct block to curDirBlock

				curDir.numEntry = 1; // set number of entries to 1 (for . entry)
				strncpy(curDir.dentry[0].name, ".", 1); // set name of . entry
				curDir.dentry[0].name[1] = '\0'; // null terminate string
				curDir.dentry[0].inode = rootInode; // set inode number of . entry
				disk_write(curDirBlock, (char*)&curDir); // write root directory to disk
		} // if
		return 0; // return success
} // fs_mount

/*
* Unmount the file system to a disk file.
*
* @param name: name of the disk file
* @return: 0 on success
*/
int fs_umount(char *name)
{
		int numInodeBlock =  (sizeof(Inode)*MAX_INODE )/ BLOCK_SIZE; // number of inode blocks in the disk
		int i, index, inode_index = 0; // index for inode array
		disk_write(0, (char*) &superBlock); // write superblock to disk
		disk_write(1, inodeMap); // write inodeMap to disk
		disk_write(2, blockMap); // write blockMap to disk

		for (i = 0; i < numInodeBlock; i++) // for each inode block
		{
				index = i+3; // inode blocks start from block 3
				disk_write(index, (char*) (inode+inode_index)); // write inode block to disk
				inode_index += (BLOCK_SIZE / sizeof(Inode)); // move to next set of inodes
		} // for
		
		// Current Directory
		disk_write(curDirBlock, (char*)&curDir); // write current directory to disk

		disk_umount(name); // unmount disk to file
} // fs_umount

/*
* Search for a file or directory in the current directory.
*
* @param name: name of the file or directory to search
* @return: inode number if found, -1 if not found
*/
int search_cur_dir(char *name)
{
		int i; // return inode. If not exist, return -1

		for (i = 0; i < curDir.numEntry; i++) // for each entry in current directory
		{
				if(command(name, curDir.dentry[i].name)) return curDir.dentry[i].inode; // if name matches, return inode number
		} // for
		return -1; // return -1 if not found
} // search_cur_dir

/*
* Create a new file in the current directory.
* Create a file with <filename> as a name in the current directory
* Fill the file with <size> of random sting
*
* @param name: name of the new file
* @param size: size of the new file in bytes
* @return: 0 on success, -1 on failure
*/
int file_create(char *name, int size)
{
		int i; // loop index

		if (size > SMALL_FILE) { // if size is greater than SMALL_FILE
				printf("Do not support files larger than %d bytes.\n", SMALL_FILE); // print error
				return -1; // return error
		} // if

		if (size < 0) { // if size is negative
				printf("File create failed: cannot have negative size\n"); // print error
				return -1; // return error
		} // if

		int inodeNum = search_cur_dir(name); // search for file in current directory
		if (inodeNum >= 0) { // if file already exists
				printf("File create failed:  %s exist.\n", name); // print error
				return -1; // return error
		} // if

		if (curDir.numEntry + 1 > MAX_DIR_ENTRY) { // if directory is full
				printf("File create failed: directory is full!\n"); // print error
				return -1; // return error
		} // if

		// Calculate Data Block Size
		int numBlock = size / BLOCK_SIZE; // calculate number of blocks needed
		if(size % BLOCK_SIZE > 0) numBlock++; // if size is not a multiple of BLOCK_SIZE, add one more block

		if (numBlock > superBlock.freeBlockCount) { // if not enough free blocks
				printf("File create failed: data block is full!\n"); // print error
				return -1; // return error
		} // if

		if (superBlock.freeInodeCount < 1) { // if not enough free inodes
				printf("File create failed: inode is full!\n"); // print error
				return -1; // return error
		} // if

		// Create File Context With Random String
		char *tmp = (char*) malloc(sizeof(int) * size + 1); // allocate memory for file data

		rand_string(tmp, size); // generate random string for file data
		printf("New File: %s\n", tmp); // print new file data

		// Get Inode Number for This File
		inodeNum = get_free_inode(); // get free inode number
		if (inodeNum < 0) { // if no free inode
				printf("File_create error: not enough inode.\n"); // invalid inode
				return -1; // return error
		} // if

		// Update I-Node Entry Info
		inode[inodeNum].type = file; // set type to file
		inode[inodeNum].owner = 1;  // pre-defined
		inode[inodeNum].group = 2;  // pre-defined
		gettimeofday(&(inode[inodeNum].created), NULL); // set creation time
		gettimeofday(&(inode[inodeNum].lastAccess), NULL); // set last access time
		inode[inodeNum].size = size; // set file size
		inode[inodeNum].blockCount = numBlock; // set number of blocks used
		inode[inodeNum].link_count = 1; // set link count to 1

		// Add a New File Into the Current Directory Entry
		strncpy(curDir.dentry[curDir.numEntry].name, name, strlen(name)); // set file name
		curDir.dentry[curDir.numEntry].name[strlen(name)] = '\0'; // null terminate string
		curDir.dentry[curDir.numEntry].inode = inodeNum; // set inode number
		printf("curdir %s, name %s\n", curDir.dentry[curDir.numEntry].name, name); // debug
		curDir.numEntry++; // increment number of entries in current directory

		// Get Data Blocks
		for (i = 0; i < numBlock; i++) // for each block needed
		{
				int block = get_free_block(); // get free block
				if (block == -1) { // if no free block
						printf("File_create error: get_free_block failed\n"); // print error
						return -1; // return error
				} // if
				
				inode[inodeNum].directBlock[i] = block; // set direct block pointer
				disk_write(block, tmp+(i*BLOCK_SIZE)); // disk write data to block
		} // for

		gettimeofday(&(inode[curDir.dentry[0].inode].lastAccess), NULL); // update inode one more time with the last access time of current directory
		printf("file created: %s, inode %d, size %d\n", name, inodeNum, size); // print success message
		free(tmp); // free allocated memory
		return 0; // return success
} // file_create

/*
* Display the contents of a file.
* Print out the content of the file
*
* @param name: name of the file to display
* @return: 0 on success, -1 on failure
*/
int file_cat(char *name)
{
		int inodeNum, i, size; // inode number, loop index, and file size
		char str_buffer[512]; // buffer to read data from disk
		char * str; // pointer to hold file data

		// Get Inode
		inodeNum = search_cur_dir(name); // search for file in current directory
		size = inode[inodeNum].size; // get file size

		// Check if Valid Input
		if (inodeNum < 0) // if file not found
		{
				printf("cat error: file not found\n"); // print error message
				return -1; // return error
		} // if
		if (inode[inodeNum].type == directory) // if inode is a directory
		{
				printf("cat error: cannot read directory\n"); // print error message
				return -1;
		} // if

		// Allocate String
		str = (char *) malloc( sizeof(char) * (size + 1)); // allocate memory for file data
		str[ size ] = '\0'; // null terminate string

		for (i = 0; i < inode[inodeNum].blockCount; i++ ) { // for each block used by the file
				int block; // block number
				block = inode[inodeNum].directBlock[i]; // get block number

				disk_read( block, str_buffer ); // read block data into buffer

				if( size >= BLOCK_SIZE ) // if remaining size is greater than or equal to BLOCK_SIZE
				{
						memcpy( str+i*BLOCK_SIZE, str_buffer, BLOCK_SIZE ); // copy full block to string
						size -= BLOCK_SIZE; // decrease remaining size
				}
				else // else if remaining size is less than BLOCK_SIZE
				{
						memcpy( str+i*BLOCK_SIZE, str_buffer, size ); // copy remaining size to string
				} // else
		} // for

		printf("%s\n", str); // print file data
		gettimeofday( &(inode[inodeNum].lastAccess), NULL ); // update last access time
		free(str); // free allocated memory
		return 0; //return success
} // file_cat

/*
* Read <size> bytes from the <offset> of the file <name>
* file_cat(char * name) will give you a lot of hints on this functions’ implementation
* Should support random read
* Obtain i-node number: Use int search_cur_dir(char *name) in fs.c
*
* Then check (refer to file_cat(char * name))
* 1) if i-node # is valid
* 2) if it is a file or directory
* 3) if size of read request is valid
*
* Inode array’s direct block has the file context
* 1) Use disk_read() in disk.c
* 2) Do not directly printf() the block context
* 3) Define a new local variable (with malloc) and use memcpy (refer to file_cat(char * name))
*
* Procedure:
* 1) Get inode number from search_cur_dir(char *name)
* 2) Validation check & error handling (e.g., size, offset, type)
* 3) Create buffer with malloc, last index = ‘\0’
* 4) Decide read block and offset with inode number, block index, and check inode struct
* 5) Disk read & memcpy, multiblock read
* 6) Update i-node for last access time
* 
* @param name: name of the file to read
* @param offset: offset to start reading from
* @param size: number of bytes to read
* @return: 0 on success, -1 on failure
*/
int file_read(char *name, int offset, int size)
{
    int inodeNum, i, fileSize; // inode number, loop index, and file size
	char str_buffer[BLOCK_SIZE]; // buffer to hold block data
	char * str; // pointer to hold file data
    int remaining, bufPos, curOffset;

    // 1. Get Inode Number From search_cur_dir(char *name)
    inodeNum = search_cur_dir(name); // search for file in current directory
	fileSize = inode[inodeNum].size; // get file size

	// 2. Validation Check and Error Handling
	// Check if I-Node Number is Valid
	if (inodeNum < 0) { // if file not found
			printf("read error: file not found\n"); // print error message
			return -1; // return error
	} // if

	// Check If It Is a File or Directory
	if (inode[inodeNum].type == directory) { // if inode is a directory
			printf("read error: cannot read directory\n"); // print error message
			return -1;
	} // if
   
	// Check If Size of Read Request is Valid
    if (offset < 0 || size < 0) { // if offset or size is negative
        printf("read error: invalid offset or size\n"); // print error message
        return -1; // return error
    } // if
    if (offset >= fileSize) { // if offset is beyond end of file
        printf("read error: offset beyond end of file\n"); // print error message
        return -1; // return error
    } // if

    // Check if requested size goes beyond end of file
    if (offset + size > fileSize) { // if offset + size is beyond end of file
        printf("read error: read size goes beyond end of file\n"); // display error message
        return -1; // return error
    } // if

    // 3. Allocate buffer With malloc (last index = ‘\0’)
	str = (char *) malloc( sizeof(char) * (size + 1)); // allocate memory for file data
    if(str == NULL) { // if malloc failed
        printf("read error: malloc failed\n"); // print error message
        return -1; // return error
    } // if
    str[size] = '\0';

	// 4. Decide Read Block and Offset with: inode number, block index, and check inode struct
    remaining = size; // bytes remaining to read
    bufPos = 0; // position in buffer
    curOffset = offset; // current offset in file

    while(remaining > 0) { // while there are bytes remaining to read
        int blockIndex = curOffset / BLOCK_SIZE; // get block index
        int blockOffset = curOffset % BLOCK_SIZE; // get offset within block
        int block = inode[inodeNum].directBlock[blockIndex]; // get block number
        int canCopy; // track number of bytes we can copy from this block

		// 5. Disk Read & memcpy, Multiblock Read
        disk_read(block, str_buffer); // read data of the block into buffer

        canCopy = BLOCK_SIZE - blockOffset; // calculate how many bytes we can copy from this block
        if(canCopy > remaining) { // if we are able to copy more than remaining bytes left to read
            canCopy = remaining; // adjust the bytes we can copy, to make it equal to remaining bytes (no over-read)
        } // if

        memcpy(str + bufPos, str_buffer + blockOffset, canCopy); // copy data from buffer to string

        remaining -= canCopy; // decrease remaining bytes to read
        bufPos    += canCopy; // increase buffer position
        curOffset += canCopy; // increase current offset in file (continue down the file)
    } // while

    /* 4. Print result */
    printf("%s\n", str);

    /* 5. Update last access time */
    gettimeofday(&(inode[inodeNum].lastAccess), NULL);

    /* 6. Cleanup */
    free(str);

    return 0;
}


/*
* Show the following metadata of the file or directory
* - I-node information
* - File or directory
* - # of blocks allocated
* - Other info stored about this file/directory
*
* @param name: name of the file to show info
* @return: 0 on success, -1 on failure
*/
int file_stat(char *name)
{
		char timebuf[28]; // buffer to hold formatted time
		int inodeNum = search_cur_dir(name); // search for file in current directory
		if (inodeNum < 0) { // if file not found
				printf("file cat error: file is not exist.\n"); // print error message
				return -1; // return error
		} // if

		printf("Inode\t\t= %d\n", inodeNum); // print inode number
		if(inode[inodeNum].type == file) printf("type\t\t= File\n"); // print file type
		else printf("type\t\t= Directory\n"); // print file type
		printf("owner\t\t= %d\n", inode[inodeNum].owner); // print owner
		printf("group\t\t= %d\n", inode[inodeNum].group); // print group
		printf("size\t\t= %d\n", inode[inodeNum].size); // print size
		printf("link_count\t= %d\n", inode[inodeNum].link_count); // print link count
		printf("num of block\t= %d\n", inode[inodeNum].blockCount); // print number of blocks
		format_timeval(&(inode[inodeNum].created), timebuf, 28); // format creation time
		printf("Created time\t= %s\n", timebuf); // print creation time
		format_timeval(&(inode[inodeNum].lastAccess), timebuf, 28); // format last access time
		printf("Last acc. time\t= %s\n", timebuf); // print last access time
} // file_stat

int file_remove(char *name)
{
		printf("Error: rm is not implemented.\n");
		return 0;
}

int dir_make(char* name)
{
		printf("Error: mkdir is not implemented.\n");
		return 0;
}

int dir_remove(char *name)
{
		printf("Error: rmdir is not implemented.\n");
		return 0;
}

int dir_change(char* name)
{
		int inodeNum, i;

		//get inode number
		inodeNum = search_cur_dir(name);
		if (inodeNum < 0) 
		{
				printf("cd error: %s does not exist\n", name);
				return -1;
		}
		if (inode[inodeNum].type != directory)
		{
				printf("cd error: % is not a directory\n", name);
				return -1;
		}

		//write parent directory (curDir) to disk
		disk_write(curDirBlock, (char*)&curDir);

		//read new directory from disk into curDir
		curDirBlock = inode[inodeNum].directBlock[0];
		disk_read(curDirBlock, (char*)&curDir);

		//update last access of directory we are changing to
		gettimeofday(&(inode[inodeNum].lastAccess), NULL);		

		return 0;
}

int ls()
{
		int i;
		for(i = 0; i < curDir.numEntry; i++)
		{
				int n = curDir.dentry[i].inode;
				if(inode[n].type == file) printf("type: file, ");
				else printf("type: dir, ");
				printf("name \"%s\", inode %d, size %d byte\n", curDir.dentry[i].name, curDir.dentry[i].inode, inode[n].size);
		}

		return 0;
}

/*
* Show file system information
* - # of free blocks
* - # of free i-nodes
*/
int fs_stat()
{
		printf("File System Status: \n"); // print header
		printf("# of free blocks: %d (%d bytes), # of free inodes: %d\n", superBlock.freeBlockCount, superBlock.freeBlockCount*512, superBlock.freeInodeCount); // print free blocks and inodes
} // fs_stat

int hard_link(char *src, char *dest)
{
		printf("Error: ln is not implemented.\n");
		return 0;
}

/*
* Execute a command with arguments.
*
* @param comm: command to execute
* @param arg1: first argument
* @param arg2: second argument
* @param arg3: third argument
* @param arg4: fourth argument
* @param numArg: number of arguments
* @return: 0 on success, -1 on failure
*/
int execute_command(char *comm, char *arg1, char *arg2, char *arg3, char *arg4, int numArg)
{

    printf ("\n"); // new line for better readability
	if(command(comm, "df")) { // if command is 'df'
				return fs_stat(); // return fs_stat

    // File Command Start (Part #1)
    } else if (command(comm, "create")) { // if command is 'create'
        if(numArg < 2) { // if not enough arguments
            printf("error: create <filename> <size>\n"); // print error message
            return -1; // return -1
        } // if
		return file_create(arg1, atoi(arg2)); // file_create(filename, size)

	} else if (command(comm, "stat")) { // if command is 'stat'
		if(numArg < 1) { // if not enough arguments
			printf("error: stat <filename>\n"); // print error message
			return -1; // return -1
		} // if
		return file_stat(arg1); // file_stat(filename)

	} else if (command(comm, "cat")) { // if command is 'cat'
		if(numArg < 1) { // if not enough arguments
			printf("error: cat <filename>\n"); // print error message
			return -1; // return -1
		} // if
		return file_cat(arg1); // file_cat(filename)

	} else if (command(comm, "read")) { // if command is 'read'
		if(numArg < 3) { // if not enough arguments
			printf("error: read <filename> <offset> <size>\n"); // print error message
			return -1; // return -1
		} // if
		return file_read(arg1, atoi(arg2), atoi(arg3)); // file_read(filename, offset, size);

	} else if (command(comm, "rm")) { // if command is 'rm'
		if(numArg < 1) { // if not enough arguments
			printf("error: rm <filename>\n"); // print error message
			return -1; // return -1
		} // if
		return file_remove(arg1); // file_remove(filename)

	} else if (command(comm, "ln")) { // if command is 'ln'
		return hard_link(arg1, arg2); // hard link. arg1: src file or dir, arg2: destination file or dir

    // Directory Command Start (Part #2)
	} else if(command(comm, "ls"))  { // if command is 'ls'
		return ls(); // list current directory

	} else if (command(comm, "mkdir")) { // if command is 'mkdir'
		if(numArg < 1) { // if not enough arguments
			printf("error: mkdir <dirname>\n"); // print error message
			return -1; // return -1
		}
		return dir_make(arg1); // dir_make(dirname)

	} else if (command(comm, "rmdir")) { // if command is 'rmdir'
		if(numArg < 1) { // if not enough arguments
			printf("error: rmdir <dirname>\n"); // print error message
			return -1; // return -1
		} // if
		return dir_remove(arg1); // dir_remove(dirname)

	} else if (command(comm, "cd")) { // if command is 'cd'
		if(numArg < 1) { // if not enough arguments
			printf("error: cd <dirname>\n"); // print error message
			return -1; // return -1
		} // if
		return dir_change(arg1); // dir_change(dirname)

	} else { // if command not found
		fprintf(stderr, "%s: command not found.\n", comm); // print error message
		return -1; // return -1
	} // if
	return 0; // return 0 on success
} // execute_command

