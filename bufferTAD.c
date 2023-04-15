#include "bufferTAD.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define BLOCK 500

char * buffer;
int fd; // file descriptor shm
sem_t* sem_id; //id del semaphore
void * alloc_mem(size_t bytes_size);

char * read_buff(){
    buffer_down();
    char * out = alloc_mem(sizeof(buffer));
    char * info = buffer_map(buffer,sizeof(buffer),PROT_READ,MAP_SHARED,0);
    strcpy(out,info);
    buffer_up();
    return out;
}

//setter function for the buffer
void load_buff(char * new_info){
    buffer_down();
    char * info = buffer_map(buffer,sizeof(buffer),PROT_READ | PROT_WRITE,MAP_SHARED,0);
    strcpy(info,new_info);
    buffer_up();
    return;
}


//wrapper for shm_open
int buffer_open(int oflag, mode_t mode){
    int aux = shm_open("buffer",oflag,mode);
    buffer = alloc_mem(BLOCK);
    if(aux == -1){
        printf("There was an error while opening the shared memory\n");
        exit(1);
    }
    fd = aux;
    return aux;
}

//wrapper for mmap
void *buffer_map(void *addr, size_t length, int prot, int flags, off_t offset){
    void * aux = mmap(addr,length,prot,flags,fd,offset);
    if (aux == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    return aux;
}

//wrapper for munmap
int buffer_unmap(void*addr, size_t length){
    munmap(addr,length);
}

//wrapper for sem_open
void buffer_sem_open(){
    sem_id = sem_open("buffer", O_CREAT);
    if(sem_id == SEM_FAILED){
        perror("There was an error while opening/creating the semaphore\n");
        exit(1);
    }
}

//wrapper for sem_post
int buffer_up(){ 
    int aux = sem_post(sem_id);
    if(aux == -1){
        perror("There was an error while leaving the semaphore\n");
        exit(1);
    }
}

//wrapper for sem_wait
int buffer_down(){ // wrapper for sem_wait
    int aux = sem_wait(sem_id);
    if(aux == -1){
        perror("There was an error while accesing the semaphore\n");
        exit(1);
    }
}

void buffer_close(){
    close(fd);
}

void * alloc_mem(size_t bytes_size) {
  void * mem_pointer = malloc(bytes_size);
  if(mem_pointer == NULL) {
    perror("Couldn't alloc spacea, check errno value");
  }
  return mem_pointer;
}   