CC = gcc
CFLAGS = -Wall -g

all:
	$(CC) $(CFLAGS) axpy_full.c -o axpy -lpthread

clean:
	rm -f axpy *.o

.PHONY: all clean

