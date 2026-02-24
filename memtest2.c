#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mymalloc.h"

int main() {
    //malloc test
    char *p1 = malloc(200);
    char *p2 = malloc(200);
    if (p1 == NULL || p2 == NULL) printf("Malloc failed\n");
    
    //Written pattern test
    memset(p1, 'A', 200);
    memset(p2, 'B', 200);
    for (int i = 0; i < 200; i++) {
        if (p1[i] != 'A' || p2[i] != 'B') printf("Failed\n");
    }

    //Reusing memory test
    free(p1);
    free(p2);
    void *p3 = malloc(400);
    if (p3 == NULL) printf("Reuse failed\n");
    free(p3);

    //Coalascing test
    void *a = malloc(100);
    void *b = malloc(100);
    void *c = malloc(100);
    free(a);
    free(b);
    free(c);
    void *big = malloc(300);
    if (big == NULL) printf("Coalascing failed\n");
    free(big);

    //Leak detection (2 objects leaked)
    malloc(50);
    malloc(10);

    //Error detection commented out because the program terminates

    //int x;
    //free(&x); 

    //int *p = malloc(sizeof(int)*2);
    //free(p + 1);

    //int *p = malloc(sizeof(int)*100);
    //int *q = p;
    //free(p);
    //free(q);

    return 0;
}