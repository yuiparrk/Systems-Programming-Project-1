CC = gcc
CFLAGS = -Wall -g

all: memtest memgrind memtest2

memtest: mymalloc.c memtest.c
	$(CC) $(CFLAGS) -o memtest mymalloc.c memtest.c

memtest2: mymalloc.c memtest2.c
	$(CC) $(CFLAGS) -o memtest2 mymalloc.c memtest2.c

memgrind: mymalloc.c memgrind.c
	$(CC) $(CFLAGS) -o memgrind mymalloc.c memgrind.c

clean:
	rm -f memtest memgrind memtest2