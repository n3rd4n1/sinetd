CC=gcc
CFLAGS=

default: sinetd test

sinetd: sinetd.c sinetd.h
	$(CC) $(CFLAGS) -o sinetd sinetd.c

test: test.c
	$(CC) $(CFLAGS) -o test test.c

clean:
	-rm -rf sinetd test
