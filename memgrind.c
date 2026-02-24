#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mymalloc.h"

void task1() {
    for (int i = 0; i < 120; i++) {
        char *ptr = malloc(1);
        free(ptr);
    }
}

void task2() {
    char *ptrs[120];
    for (int i = 0; i < 120; i++) {
        ptrs[i] = malloc(1);
    }
    for (int i = 0; i < 120; i++) {
        free(ptrs[i]);
    }
}

void task3() {
    char *ptrs[120];
    int allocated[120] = {0};
    int loc = 0;
    int total_mallocs = 0;

    while (total_mallocs < 120) {
        int choice = rand() % 2;

        if (choice == 0 && total_mallocs < 120) {
            ptrs[loc] = malloc(1);
            allocated[loc] = 1;
            loc++;
            total_mallocs++;
        } else if (choice == 1 && loc > 0) {
            loc--;
            free(ptrs[loc]);
            allocated[loc] = 0;
        }
    }

    while (loc > 0) {
        loc--;
        free(ptrs[loc]);
    }
}

//allocate blocks of increasing size
void task4() {
    char *ptrs[60];
    for (int i = 0; i < 60; i++) {
        ptrs[i] = malloc(i + 1);
    }
    //free even indices
    for (int i = 0; i < 60; i += 2) free(ptrs[i]);
    //free odd indices
    for (int i = 1; i < 60; i += 2) free(ptrs[i]);
}

//allocate a large amount of blocks and then free them all at once
void task5() {
    void *ptrs[32];
    for(int i = 0; i < 32; i++) {
        ptrs[i] = malloc(64);
    }
    for(int i = 0; i < 32; i++) {
        free(ptrs[i]);
    }
}

int main() {
    struct timeval start, end;
    long total_time = 0;

    gettimeofday(&start, NULL);

    for (int i = 0; i < 50; i++) {
        task1();
        task2();
        task3();
        task4();
        task5();
    }

    gettimeofday(&end, NULL);

    total_time = ((end.tv_sec - start.tv_sec) * 1000000LL + end.tv_usec - start.tv_usec);

    printf("Average time per workload: %ld microseconds\n", total_time / 50);

    return 0;
}