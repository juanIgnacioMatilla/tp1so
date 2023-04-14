#ifndef BUFFERTAD_H
#define BUFFERTAD_H

#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <sys/mman.h>
#include <semaphore.h>

typedef struct{
    char * name;
    char * md5;
    int pid;
}result_data;

void create_sem(char * name);
void load_name(char * new);
void load_md5(char * new);
void load_pid(int new);
int buffer_open(const char *name, int oflag, mode_t mode);
void *buffer_map(void *addr, size_t length, int prot, int flags, off_t offset);
int buffer_unmap(void *addr, size_t length);
int buffer_up();
int buffer_down();
void buffer_close();

#endif