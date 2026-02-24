#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mymalloc.h"

//total size of heap
#define MEMLENGTH 4096

//metadata of header
typedef struct {
    int size;
    int is_allocated; //1 = allocated 0 = free
} header_t;

static union {
    char bytes[MEMLENGTH];
    double not_used; //double so that the heap is aligned in 8 bytes
} heap;

static int initialized = 0;

//runs at program exit
static void leak_detector() {
    int current_pos = 0;
    int leaked_bytes = 0;
    int leaked_objects = 0;

    //next 2 lines used often to traverse the entire heap
    while (current_pos < MEMLENGTH) {
        header_t *header = (header_t *)&heap.bytes[current_pos];
        //if block is still allocated, it is leaked
        if (header->is_allocated) {
            leaked_bytes += header->size;
            leaked_objects++;
        }
        //next block
        current_pos += sizeof(header_t) + header->size;
    }

    if (leaked_objects > 0) {
        fprintf(stderr, "mymalloc: %d bytes leaked in %d objects.\n", leaked_bytes, leaked_objects);
    }
}

static void initialize_heap() {
    //first header at beginning of heap
    header_t *first_chunk = (header_t *)heap.bytes;
    //the heap is 1 free block
    first_chunk->size = MEMLENGTH - sizeof(header_t);
    first_chunk->is_allocated = 0;
    initialized = 1;
    //leak detection when program exits
    atexit(leak_detector);
}

void *mymalloc(size_t size, char *file, int line) {
    //intializes heap first time mymalloc is called
    if (!initialized) {
        initialize_heap();
    }
    if (size <= 0) {
        return NULL;
    }

    //rounds up the size to a multiple of 8
    size_t aligned_size = size;
    if (aligned_size % 8 != 0) {
        aligned_size = aligned_size + (8 - (aligned_size % 8));
    }

    //goes through the heap to find a free block
    int current_pos = 0;
    while (current_pos < MEMLENGTH) {
        header_t *header = (header_t *)&heap.bytes[current_pos];
        
        //is block free and big enough
        if (!header->is_allocated && header->size >= (int)aligned_size) {
            //can the block be split
            if (header->size >= (int)(aligned_size + sizeof(header_t) + 8)) {
                int old_total_size = header->size;

                //allocate the first part of the block
                header->size = (int)aligned_size;
                header->is_allocated = 1;

                //make new free block with leftover space
                int next_pos = current_pos + sizeof(header_t) + aligned_size;
                header_t *next_header = (header_t *)&heap.bytes[next_pos];
                next_header->size = old_total_size - aligned_size - sizeof(header_t);
                next_header->is_allocated = 0;
            } else {
                //not enough space just allocate the entire block
                header->is_allocated = 1;
            }
            //return the pointer to payload
            return (void *)&heap.bytes[current_pos + sizeof(header_t)];
        }
        //move to next block
        current_pos += sizeof(header_t) + header->size;
    }
    //no block that fits found
    fprintf(stderr, "malloc: Unable to allocate %zu bytes (%s:%d)\n", size, file, line);
    return NULL;
}

void myfree(void *ptr, char *file, int line) {
    if (ptr == NULL) {
        return;
    }
    //is pointer inside the heap
    if ((char *)ptr < heap.bytes + sizeof(header_t) || (char *)ptr >= heap.bytes + MEMLENGTH) {
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }

    int current_pos = 0;
    header_t *target = NULL;

    //finds the block whose payload matches ptr
    while (current_pos < MEMLENGTH) {
        header_t *header = (header_t *)&heap.bytes[current_pos];
        void *payload_ptr = (void *)&heap.bytes[current_pos + sizeof(header_t)];

        if (payload_ptr == ptr) {
            target = header;
            break;
        }
        current_pos += sizeof(header_t) + header->size;
    }

    //checks if the pointer is valid and not freed
    if (target == NULL || target->is_allocated == 0) {
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }

    //changes block to free
    target->is_allocated = 0;

    //coalesce blocks next to each other
    current_pos = 0;
    while (current_pos < MEMLENGTH) {
        header_t *curr = (header_t *)&heap.bytes[current_pos];
        int next_pos = current_pos + sizeof(header_t) + curr->size;

        if (next_pos < MEMLENGTH) {
            header_t *next = (header_t *)&heap.bytes[next_pos];
            //merge current and next if they are both free
            if (curr->is_allocated == 0 && next->is_allocated == 0) {
                curr->size += sizeof(header_t) + next->size;
                continue;
            }
        }
        //move to next block
        current_pos += sizeof(header_t) + curr->size;
    }
}