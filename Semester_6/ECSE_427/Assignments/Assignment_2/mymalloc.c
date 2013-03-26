
#include "mymalloc.h"

/*
This function returns a void* that we can assign to any C pointer. If memory could
not be allocated, then my_malloc() returns NULL and sets a global error string 
variable. To reduce the calls to sbrk(), rather than allocating exactly the 
number of bytes, my_malloc() increases the program break in larger units and 
puts the excess memory onto the free list. Furthermore, when my_malloc() allocates
the block, it allocates extra bytes to hold an integer containing the size of the 
block. The integer is located at the beginning of the block; the address actually
returned to the caller points to the location just pas this lenght value.
*/
void *my_malloc(int size){

}

/*
This function deallocates the block of memory pointed by the ptr argument. The ptr
should be an address previously allocated by the Memory Allocation package. The 
function does not lower the program break. It adds the block of freed memory to the
free list so it could be recycled by future calls to malloc(). Furthermore, the 
function reduces thte program break if the top free block is larger than 128Kbytes.
*/
void my_free(void *ptr){

}

/*
This function specifies the memory allocation policy. These can be either
firt fit or best fit.
*/
void my_mallopt(int policy){

}

/*
This function prints statistics about the memory allocation performed so far by
the library. The stats include: total number of bytes allocated, total free space,
largest contiguous free space, etc...
*/
void my_mallinfo(){

}

int main(){
  
}