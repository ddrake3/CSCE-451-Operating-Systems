CC = gcc

CFLAGS = -c -Wall

all: myecho mycat

myecho: myecho.o
	$(CC) myecho.o -o myecho

myecho.o: myecho.c
	$(CC) $(CFLAGS) myecho.c

mycat: mycat.o
	$(CC) mycat.o -o mycat

mycat.o: mycat.c
	$(CC) $(CFLAGS) mycat.c

clean:
	rm -rf *o mycat
