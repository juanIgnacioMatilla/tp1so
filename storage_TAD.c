#include "storage_TAD.h"


#define BYTES_BLOCK 10;

size_t count;

errno = 0;

typedef struct {
  file_dir_pair * pair;
  file_dir_pair_node * next;
} file_dir_pair_node;


file_dir_pair_node * next;

file_dir_pair_node * list;

file_dir_pair_node * create_next(char * dirs[]);

void * alloc_mem(size_t bytes_size);

void load_file_dirs(char * dirs[]) {

  size_t i;
  while(dirs != NULL) {
    if(next == NULL) 
      next = create_next();
    if(errno != 0)  
      return;
    if(list == NULL)
        list = next;
    if(next->pair->dirs == 0) {

      strcopy(next->pair->file_dir1, *dirs);
      if(errno != 0)
        return NULL;
      dirs+=sizeof(char*);
      next->pair->dirs += 1; 
    } else {
      strcopy(next->pair->file_dir2, *dirs);
      if(errno != 0)
        return NULL;
      dirs+=sizeof(char*);
      next->pair->dirs += 1; 
      next = next->next;
    }

  }
}


file_dir_pair_node * create_next() {

  next = (file_dir_pair_node *) alloc_mem(sizeof(file_dir_pair_node));
  if (errno != 0) {
    perror("Failed to create file_dir_pair_node");
    return next;
  }

  next->pair = (file_dir_pair *) alloc_mem(sizeof(file_dir_pair));
  if (errno != 0) {
    perror("Failed to create file_dir_pair");
    return next;
  }
  next->pair->dirs = 0;
  return next;
}


void * alloc_mem(size_t bytes_size) {
  void * mem_pointer = malloc(size);
  if(next == NULL) {
    perror("Couldn't alloc spacea, check errno value");
  }
  return mem_pointer;
}

