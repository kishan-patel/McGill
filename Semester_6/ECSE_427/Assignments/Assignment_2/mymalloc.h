#define FIRST_FIT                         1
#define BEST_FIT                          2

extern char *my_malloc_error

void *my_malloc(int size);
void my_free(void *ptr);
void my_mallopt(int policy);
void my_mallinfo();

