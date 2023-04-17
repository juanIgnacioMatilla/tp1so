#ifndef BUFFERTAD_H
#define BUFFERTAD_H

#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <sys/mman.h>
#include <semaphore.h>
#include <stdlib.h>

typedef struct buffTAD * smh_data;

void create_sem(char * name);
void load_buff(smh_data info, char * data);
char * read_buff();
void buffer_open(smh_data info);
void buffer_map(smh_data info);
int buffer_unmap(void *addr, size_t length);
void buffer_sem_open();
void buffer_up(smh_data info);
void buffer_down(smh_data info);
void buffer_close(smh_data info);
smh_data start_shm(mode_t new_mode);
void load_max_files(smh_data info, int max);

#endif