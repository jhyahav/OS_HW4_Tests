#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
void initQueue(void);
void destroyQueue(void);
void enqueue(const void*);
void* dequeue(void);
bool tryDequeue(void**);
size_t size(void);
size_t waiting(void);
size_t visited(void);
