BINARY=master
SLAVE_BINARY=slave 
VIEW_BINARY=vista
CODEDIRS=.
INCDIRS=. ./

CC=gcc
CFLAGS= -Wall -std=c99 -D_GNU_SOURCE -pthread -lrt #-g
CFILES = master.c bufferTAD.c
OBJECTS=$(patsubst %.c,%.o,$(CFILES)) 

all: $(BINARY) $(SLAVE_BINARY) $(VIEW_BINARY)

$(BINARY): $(CFILES)
	$(CC) $(CFLAGS) $^ -o $@ 

$(SLAVE_BINARY): slave.c
	$(CC) $(CFLAGS) $^ -o $@ 

$(VISTA_BINARY): bufferTAD.o
	$(CC) $(CFLAGS) -c vista.c 
	$(CC) vista.o $^ -lrt -pthread -o $@ 

clean: 
	rm -rf $(BINARY) $(SLAVE_BINARY) $(VIEW_BINARY) *.o *.d 