#ifndef BUFFERTAD_H
#define BUFFERTAD_H

#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <sys/mman.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
typedef struct buffTAD * shm_data;

shm_data start_shm(char * new_name, int new_mode);
void load_max_files(shm_data info, int max);
void load_buff(shm_data info, char * data);
int read_max_files(shm_data info);
void read_buff(shm_data info, char * out);
void buffer_open(shm_data info);
void buffer_map(shm_data info,int mode);
void print_data(shm_data info);
int buffer_unmap(void *addr, size_t length);
void buffer_sem_open();
void buffer_up(shm_data info);
void buffer_down(shm_data info);
void buffer_close(shm_data info);
void buffer_free(shm_data info);
void buffer_unlink(shm_data info);
#endif
