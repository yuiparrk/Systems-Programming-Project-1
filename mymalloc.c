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
    
}