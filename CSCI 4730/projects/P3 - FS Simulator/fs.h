#include <sys/time.h> // for struct timeval
#include "disk.h" // for disk operations

#define MAX_BLOCK 4096 // total number of blocks on disk - 4096 blocks
#define MAX_INODE 512 // total number of bytes per i-node block - 512 bytes (4 inodes per block)
#define MAX_FILE_NAME 20 // max file name length is 20 characters
#define SMALL_FILE 6144 // max file size - 6KB (6144 bytes)
#define MAGIC_NUMBER 0x1234FFFF // magic number to identify our file system
#define MAX_DIR_ENTRY BLOCK_SIZE / sizeof(DirectoryEntry) // max number of directory entries per block

typedef enum {file, directory} TYPE; // enumerate file type - file or directory

/*
* SuperBlock Structure
*/
typedef struct {
		int magicNumber; // to identify our file system - 4 bytes
		int freeBlockCount; // track number of free blocks - 4 bytes
		int freeInodeCount; // track number of free inodes - 4 bytes
		char padding[500]; // padding to make superblock size 512 bytes
} SuperBlock; // 512 byte SuperBlock Structure

/*
* Inode Structure
*/
typedef struct {
		TYPE type; // file or directory - 4 bytes
		int owner; // owner id - 4 bytes
		int group; // group id - 4 bytes
		struct timeval lastAccess; // last access time - 16 bytes
		struct timeval created; // creation time - 16 bytes
		int size; // size of the file - max is 6KB (6144 bytes) - 4 bytes
		int blockCount; // number of blocks used for the file - max is 12 blocks - 4 bytes
		int directBlock[12]; // direct block pointers - max is 12 direct block pointers - 48 bytes
		int link_count; // for hardlink - 4 bytes
		char padding[16]; // padding to make inode emtry size 128 bytes (to align takes 8 bytes)
} Inode; // 128 byte Inode Entry Structure

/*
* Directory Entry Structure
*/
typedef struct {
		char name[MAX_FILE_NAME]; // file or directory name - max is 20 characters - 20 bytes
		int inode; // inode number - 4 bytes
} DirectoryEntry; // 24 byte Directory Entry Structure

/*
* Directory Block Structure that adjusts the directory entries
*/
typedef struct {
		int numEntry; // number of directory entries - 4 bytes
		DirectoryEntry dentry[MAX_DIR_ENTRY]; // directory entries - max 25 entries - 24 bytes each
		char padding[4]; // padding to make directory block size 512 bytes
} Dentry; // 512 byte Directory Block Structure

/*
* Holds structure of superBlock, inodeMap, and blockMap
*/
extern SuperBlock superBlock; // superblock
extern char inodeMap[MAX_INODE / 8]; // inode bitmap
extern char blockMap[MAX_BLOCK / 8]; // block bitmap

/*
* Function Prototypes
*/
int fs_mount(char *name); /* mount file system */
int fs_umount(char *name); /* unmount file system */
int execute_command(char *comm, char *arg1, char *arg2, char *arg3, char *arg4, int numArg); /* execute command */