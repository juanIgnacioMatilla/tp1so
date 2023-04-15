#ifndef BUFFERTAD_H
#define BUFFERTAD_H

#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <sys/mman.h>
#include <semaphore.h>

void create_sem(char * name);
void load_buff(char * new);
char * read_buff();
int buffer_open(int oflag, mode_t mode);
void *buffer_map(void *addr, size_t length, int prot, int flags, off_t offset);
int buffer_unmap(void *addr, size_t length);
void buffer_sem_open();
int buffer_up();
int buffer_down();
void buffer_close();

#endif