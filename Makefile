CC = gcc
CFLAGS = -Wall -g

all: memtest memgrind

memtest: mymalloc.c memtest.c
	$(CC) $(CFLAGS) -o memtest mymalloc.c memtest.c

memgrind: mymalloc.c memgrind.c
	$(CC) $(CFLAGS) -o memgrind mymalloc.c memgrind.c

clean:
	rm -f memtest memgrind