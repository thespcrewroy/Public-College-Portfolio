#define BLOCK_SIZE 512 // total number of bytes per block - 512 bytes
#define MAX_BLOCK 4096 // total number of blocks on disk - 4096 blocks

extern char disk[MAX_BLOCK][BLOCK_SIZE]; // create simulated disk as a 2D array

/*
* Function Prototypes
*/
int disk_read(int block, char *buf); /* read a block from disk */
int disk_write(int block, char *buf); /* write a block to disk */

int disk_mount(char *name); /* mount the disk */
int disk_umount(char *name); /* unmount the disk */

