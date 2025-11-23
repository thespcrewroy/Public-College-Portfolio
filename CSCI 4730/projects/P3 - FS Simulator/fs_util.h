/*
* Function Prototypes
*/
int rand_string(char *str, size_t size); /* generate random string */
void set_bit(char *array, int index, char value); /* set bit in bitmap */
char get_bit(char *array, int index); /* get bit from bitmap */
int get_free_inode(); /* get free inode */
int get_free_block(); /* get free block */
int format_timeval(struct timeval *tv, char *buf, size_t sz); /* format timeval */