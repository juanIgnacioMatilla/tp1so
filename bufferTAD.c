#include "bufferTAD.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>


result_data * info;
int fd; // file descriptor shm
const char * sem_id; //id del semaphore
void * alloc_mem(size_t bytes_size);

void create_sem(char * name){
    sem_id = alloc_mem(sizeof(name));
    strcpy(sem_id,name);
}

void load_name(char * new){
    info->name = alloc_mem(sizeof(new));
    strcpy(info->name,new);
    return;
}

void load_md5(char * new){
    info->md5 = alloc_mem(sizeof(new));
    strcpy(info->md5,new);
    return;
}

void load_pid(int new){
    info->pid = new;
    return;
}

int buffer_open(const char *name, int oflag, mode_t mode){
    int aux = shm_open(name,oflag,mode);
    if(aux == -1){
        printf("There was an error while opening the shared memory\n");
        exit(1);
    }
    fd = aux;
    return aux;
}

void *buffer_map(void *addr, size_t length, int prot, int flags, off_t offset){
    void * aux = mmap(addr,length,prot,flags,fd,offset);
    if (aux == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    return aux;
}

int buffer_unmap(void*addr, size_t length){
    munmap(addr,length);
}

int buffer_up(){
    sem_post(sem_id);
}

int buffer_down(){
    sem_wait(sem_id);
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