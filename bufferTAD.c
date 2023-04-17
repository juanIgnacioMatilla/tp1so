#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include "bufferTAD.h"

#define MAX_SIZE 512

typedef struct buffTAD{
    char * buff; //where the data is going to be stored
    int fd; //file descriptor for the shared mem
    sem_t * sem_id; //id for the sempahore
    void * last_seen;
    void * first_pos;
    mode_t mode;
}buffTAD;

smh_data start_shm(mode_t new_mode){
    smh_data new_shm = calloc(1,sizeof(buffTAD));
    if(new_shm == NULL){
        perror("There was an error while allocating space for the shared memory\n");
    }
    new_shm->mode = new_mode;
    new_shm->sem_id = sem_open("buff_sem", O_CREAT, S_IRUSR|S_IWUSR, 0);
    if(new_shm->sem_id ==  SEM_FAILED)
        perror("There was an error while creating the semaphore\n");
    return new_shm;
}

//wrapper for shm_open
void buffer_open(smh_data info){
    info->fd = shm_open("buffer",O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if(info->fd == -1){
        perror("There was an error while opening the shared memory\n");
    }
    if(ftruncate(info->fd,512) == SEM_FAILED)
        perror("There was an error while creating space for the shared memory\n");
}

void buffer_up(smh_data info ){ 
    int aux = sem_post(info->sem_id);
    if(aux == -1){
        perror("There was an error while leaving the semaphore\n");
        exit(1);
    }
}

void buffer_down(smh_data info){ // wrapper for sem_wait
    int aux = sem_wait(info->sem_id);
    if(aux == -1){
        perror("There was an error while accesing the semaphore\n");
        exit(1);
    }
}

//wrapper for mmap
void buffer_map(smh_data info){
    void * pos = mmap(NULL, 512,info->mode,MAP_SHARED,info->fd,0);
    if(pos == MAP_FAILED){
        perror("There was an error while mapping the shared memory\n");
    }
    info->first_pos = pos;
    info->last_seen = pos;
}

void buffer_close(smh_data info){
    if(sem_close(info->sem_id) == -1)
        perror("There was an error while closing the semaphore\n");
    if(munmap(info->first_pos,512) == -1)
        perror("There was an error while unmapping the shared memory\n");
    if(close(info->fd) == -1)
        perror("There was an error while closing the shared memory\n");
}

void load_max_files(smh_data info, int max){
    int * first = info->first_pos;
    *first = max;
    info->last_seen+=sizeof(int);
    if(sem_post(info->sem_id) == -1)
        perror("There was a problem while accessing the semaphore\n");
}

void load_buff(smh_data info, char * data){
    if(info->first_pos == NULL)
        perror("Shared memory not initialized\n");
    int size = strlen(data);
    int * current = info->last_seen;
    *current = size;
    info->last_seen += sizeof(int);
    sprintf(info->last_seen,"%s",data);
    buffer_up(info);
}


