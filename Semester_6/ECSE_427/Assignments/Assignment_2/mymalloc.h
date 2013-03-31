#define FIRST_FIT                         1
#define BEST_FIT                          2

extern char *my_malloc_error;

typedef struct block_info{
  struct block_info* next; 
  struct block_info* prev;
  void* address;
  int availability;
  int size;
}block_info;

void* my_malloc(int size);
void my_free(void *ptr);
void my_mallopt(int policy);
void my_mallinfo();
void* allocate_using_first_fit();
void* allocate_using_best_fit();
void* allocateExtraMemory(block_info* block, int requestedSize);
void* allocateMemoryAndStoreRemaining(block_info* block, int requestedSize);
void* createMoreMemory(block_info* block, int requestedSize);