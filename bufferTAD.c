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

int ftruncate(int fildes, off_t length);

typedef struct buffTAD{
    char name[7];
    int fd; //file descriptor for the shared mem
    sem_t * sem_id; //id for the sempahore
    void * last_seen;
    void * first_pos;
    int mode;
}buffTAD;

//initialize the shared mem
shm_data start_shm(char * new_name, int new_mode){
    shm_data new_shm = calloc(1,sizeof(buffTAD));
    if(new_shm == NULL){
        perror("There was an error while allocating space for the shared memory\n");
    }

    strncpy(new_shm->name,new_name,strlen(new_name));
    new_shm->mode = new_mode;
    new_shm->sem_id = sem_open(new_name, O_CREAT, S_IRUSR|S_IWUSR, 0);
    //printf("%d\n",(int)new_shm->sem_id);
    if(new_shm->sem_id ==  SEM_FAILED)
        perror("There was an error while creating the semaphore\n");
    return new_shm;
}

void print_data(shm_data info){
    printf("%s",info->name);
}

//wrapper for shm_open
void buffer_open(shm_data info){
    info->fd = shm_open(info->name,O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if(info->fd == -1){
        perror("There was an error while opening the shared memory\n");
    }
    if(ftruncate(info->fd,512) != 0)
        perror("There was an error while creating space for the shared memory\n");
}

//wrapper for sem_post
void buffer_up(shm_data info ){ 
    if(sem_post(info->sem_id) == -1){
        perror("There was an error while leaving the semaphore\n");
        exit(1);
    }
}

//wrapper for sem_wait
void buffer_down(shm_data info){
    if(sem_wait(info->sem_id) == -1){
        perror("There was an error while accesing the semaphore\n");
        exit(1);
    }
}

//wrapper for mmap
void buffer_map(shm_data info,int mode){
    void * pos = mmap(NULL, 512,mode,MAP_SHARED,info->fd,0);
    if(pos == MAP_FAILED){
        perror("There was an error while mapping the shared memory\n");
    }
    info->first_pos = pos;
    info->last_seen = pos;
}

//closes the semaphore, unmaps the shared mem, and closes the shared mem
void buffer_close(shm_data info){
    if(sem_close(info->sem_id) == -1)
        perror("There was an error while closing the semaphore\n");
    if(munmap(info->first_pos,512) == -1)
        perror("There was an error while unmapping the shared memory\n");
    if(close(info->fd) == -1)
        perror("There was an error while closing the shared memory\n");
}

void buffer_free(shm_data info){
    free(info);
}

//write in the first pos of the shared mem the total amount of files that will be processed
void load_max_files(shm_data info, int max){
    int * first = (int*) info->first_pos;
    *first = max;
    info->last_seen+=sizeof(int);
    buffer_up(info);
}

//load the shared mem with the result data
void load_buff(shm_data info, char * data){
    if(info->first_pos == NULL)
        perror("Shared memory not initialized\n");
    int size = strlen(data);
    int * current = info->last_seen;
    *current = size;
    info->last_seen += sizeof(int);
    sprintf(info->last_seen,"%s",data);
    buffer_up(info);
}

//returns the amount of files that where given to process to the master
int read_max_files(shm_data info){
    buffer_down(info);
    printf("seras castigada\n");
    int * pos = (int *)info->last_seen;
    int out = *pos;
    info->last_seen+= sizeof(int);
    return out;
}

//loads in out what is inside the buffer
void read_buff(shm_data info, char * out){
    buffer_down(info);
    size_t length = *((int*)info->last_seen);
    info->last_seen += sizeof(int);
    if(snprintf(out,length,"%s",(char *)info->last_seen) < 0)
        perror("There was an error while reading from the shared memory\n");
    info->last_seen += length;
}
