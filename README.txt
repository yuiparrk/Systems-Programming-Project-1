Daniel Park
dp1555

Test Plan:
The following scenarios were tested:
1. Making sure that mymalloc returns a valid pointer and myfree
marks that block as free.
2. Making sure that the mymalloc can split a large block with excess 
memory into a smaller block to be used later and save space.
3. Making sure that myfree can merge adjacent free blocks so that
large allocations can be done on previously used space.
4. Detectable Errors given in section 2.1:
- calling myfree with a address not from mymalloc
- calling myfree with a adress not at the start of a block
- calling myfree 2 times on the same pointer
5. Making sure atexit finds and reports any memory that was not freed
at the end of the program.
6. Using memgrind.c to test how long the average time of each workload
was

Description of Test Programs:
Other than the errors in section 2.1 Detectable Errors, this program was
tested with 5 different tasks, 3 of them provided by the instructions and
2 of them created from the file memgrind.c. These 5 tasks are repeated 50
times and the time it took per workload is measured. The first task 
allocates 1 byte and then frees it 120 times. This tests if there are any
problems with allocating and then freeing. The second task allocates 120 
blocks of 1 byte each, stores them in a array and then frees all of it. 
This tests the ability for the program to do many allocations and then
many deallocations. The third task randomly chooses allocating 1 byte or freeing
a allocated block until 120 allocations have been performed. Then, all
the occupied blocks are freed at the end. This tests random and unpredictable
actions. The fourth task allocates 60 blocks with increasing sizes in a array
and then frees first the pointers with even indicies, then the pointers
with odd indices. This tests allocation of different sizes and non-incremental
freeing. The fifth task allocates 32 blocks each 64 bytes and frees all of
it. This tests the allocation of large blocks and tests coalescing because
this same task is run multiple times.

There are no arguments because the test programs run automatically
once it is executed.

Design Notes:
The header is defined with this segment of code:
typedef struct {
    int size;
    int is_allocated;
} header_t;

size refers to the size of the payload and is_allocated lets us know
if the block is free or not defined by a 1 if it is occupried and 0 if
it is empty

The heap is defined with this segment of code:

static union {
    char bytes[MEMLENGTH];
    double not_used;
} heap;

This makes the heap aligned to a 8 byte boundary

The leak_detector function is always run at program exit. The function
traverses the entire heap to look for blocks that are still allocated. If
it is, it adds the amount of memory still allocated and adds it to
leaked_bytes and increments leaked_objects. Then, it moves onto the 
next block and repeats the process. At the end, if leaked_objects is
greater than 0, it prints out the amount of bytes leaked and the amount
of objects leaked.

The intialize_heap function is first run when mymalloc is called. It 
sets the first header at the beginning and sets the length of the heap
as the entire heap minus the header just created. It sets the intialized 
variable to 1 so that the heap isnt set up again and calls leak_detector
at program exit.

The mymalloc function checks if the heap needs to be intialized. If it is
not, it calls intialize_heap. Then, it takes the requested size and rounds
it up to the closest multiple of 8 to make sure everything is aligned. 
Then, it traverses the entire heap to find a block that is big enough
to fit the size. If there is enough space that is found, it also checks
if there is more space that can be split meaning there is enough room
for a new header and at least 8 bytes of the payload. If that is the case,
the first part is marked as occupied and the second part becomes a 
free block with leftover space and returns a pointer to the memory right 
after the pointer. If not, the entire block is marked as
occupied and returns a pointer to the memory right after the pointer. If
there is no space where the block can fit, it prints out a error message
and returns NULL.

The myfree function first checks if the pointer is valid and if the pointer
is inside the heap. Then, it traverses the entire heap block by block and
finds the pointer. Then, it checks if the pointer is valid and if it is
occupied. Then, it changes the block to show that it is freed. It also
traverses the heap again and if the current block and the next block is
free, it merges them to free up free space.