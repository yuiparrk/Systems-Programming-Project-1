#include <stdlib.h>
#include <stdio.h>
#include "mymalloc.h"
#define MEMLENGTH 4096

//intialize heap
static union {
    char bytes[MEMLENGTH];
    double not_used;
} heap;

static int intialized = 0;

//header
typedef struct {
    size_t size;
    int free;
} header;

void intialize_heap() {
    header *first = (header *)heap.bytes;
    first->size = MEMLENGTH;
    first->free = 1;
    intialized = 1;
}

size_t round_up(size_t size) {
    if (size % 8 != 0) {
        size = size + (8 - (size % 8));
    }
}