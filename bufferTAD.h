#ifndef BUFFERTAD_H
#define BUFFERTAD_H

#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <sys/mman.h>
#include <semaphore.h>
#include <stdlib.h>

typedef struct buffTAD * shm_data;

void create_sem(char * name);
void load_max_files(shm_data info, int max);
void load_buff(shm_data info, char * data);
int read_max_files(shm_data info);
void read_buff(shm_data info, char * out);
void buffer_open(shm_data info);
void buffer_map(shm_data info);
int buffer_unmap(void *addr, size_t length);
void buffer_sem_open();
void buffer_up(shm_data info);
void buffer_down(shm_data info);
void buffer_close(shm_data info);
shm_data start_shm(mode_t new_mode);


#endif