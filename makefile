CC = gcc
CFLAGS = -Wall -std=c99
LIBS = -lssl -lcrypto

all: slave master vista

slave: slave.c 
	$(CC) $(CFLAGS) -o slave slave.c $(LIBS)
master: master.c 
	$(CC) $(CFLAGS) -o master master.c
vista: vista.c 
	$(CC) $(CFLAGS) -o vista vista.c

clean:
	rm -f slave master vista