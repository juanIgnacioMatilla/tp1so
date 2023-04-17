CC = gcc
CFLAGS = -Wall -std=c99 -lrt -pthread
LIBS = 

all: slave master vista

slave: slave.c 
	$(CC) $(CFLAGS) -o slave slave.c
master: master.c 
	$(CC) $(CFLAGS) -o master master.c bufferTAD.c bufferTAD.h
vista: vista.c 
	$(CC) $(CFLAGS) -o vista vista.c bufferTAD.c bufferTAD.h

clean:
	rm -f slave master vista output.txt