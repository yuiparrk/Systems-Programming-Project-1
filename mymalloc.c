#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mymalloc.h"

#define MEMLENGTH 4096

typedef struct {
    int size;
    int is_allocated;
} header_t;

static union {
    char bytes[MEMLENGTH];
    double not_used;
} heap;

static int initialized = 0;

static void leak_detector() {
    int current_pos = 0;
    int leaked_bytes = 0;
    int leaked_objects = 0;

    while (current_pos < MEMLENGTH) {
        header_t *header = (header_t *)&heap.bytes[current_pos];
        if (header->is_allocated) {
            leaked_bytes += header->size;
            leaked_objects++;
        }
        current_pos += sizeof(header_t) + header->size;
    }

    if (leaked_objects > 0) {
        fprintf(stderr, "mymalloc: %d bytes leaked in %d objects.\n", leaked_bytes, leaked_objects);
    }
}

static void initialize_heap() {
    header_t *first_chunk = (header_t *)heap.bytes;
    first_chunk->size = MEMLENGTH - sizeof(header_t);
    first_chunk->is_allocated = 0;
    initialized = 1;
    atexit(leak_detector);
}

void *mymalloc(size_t size, char *file, int line) {
    if (!initialized) {
        initialize_heap();
    }
    if (size <= 0) {
        return NULL;
    }

    size_t aligned_size = size;
    if (aligned_size % 8 != 0) {
        aligned_size = aligned_size + (8 - (aligned_size % 8));
    }

    int current_pos = 0;
    while (current_pos < MEMLENGTH) {
        header_t *header = (header_t *)&heap.bytes[current_pos];

        if (!header->is_allocated && header->size >= (int)aligned_size) {
            if (header->size >= (int)(aligned_size + sizeof(header_t) + 8)) {
                int old_total_size = header->size;
                header->size = (int)aligned_size;
                header->is_allocated = 1;

                int next_pos = current_pos + sizeof(header_t) + aligned_size;
                header_t *next_header = (header_t *)&heap.bytes[next_pos];
                next_header->size = old_total_size - aligned_size - sizeof(header_t);
                next_header->is_allocated = 0;
            } else {
                header->is_allocated = 1;
            }

            return (void *)&heap.bytes[current_pos + sizeof(header_t)];
        }
        current_pos += sizeof(header_t) + header->size;
    }

    fprintf(stderr, "malloc: Unable to allocate %zu bytes (%s:%d)\n", size, file, line);
    return NULL;
}

void myfree(void *ptr, char *file, int line) {
    if (ptr == NULL) {
        return;
    }
    if ((char *)ptr < heap.bytes + sizeof(header_t) || (char *)ptr >= heap.bytes + MEMLENGTH) {
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }

    int current_pos = 0;
    header_t *target = NULL;

    while (current_pos < MEMLENGTH) {
        header_t *header = (header_t *)&heap.bytes[current_pos];
        void *payload_ptr = (void *)&heap.bytes[current_pos + sizeof(header_t)];

        if (payload_ptr == ptr) {
            target = header;
            break;
        }
        current_pos += sizeof(header_t) + header->size;
    }

    if (target == NULL || target->is_allocated == 0) {
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }

    target->is_allocated = 0;

    current_pos = 0;
    while (current_pos < MEMLENGTH) {
        header_t *curr = (header_t *)&heap.bytes[current_pos];
        int next_pos = current_pos + sizeof(header_t) + curr->size;

        if (next_pos < MEMLENGTH) {
            header_t *next = (header_t *)&heap.bytes[next_pos];
            if (curr->is_allocated == 0 && next->is_allocated == 0) {
                curr->size += sizeof(header_t) + next->size;
                continue;
            }
        }
        current_pos += sizeof(header_t) + curr->size;
    }
}