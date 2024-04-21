CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -g

build:
	$(CC) $(CFLAGS) main.c -o sfl

run_sfl:
	./sfl

clean:
	rm sfl