#ifndef
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#endif

typedef struct {

  char * file_dir1;
  char * file_dir2;
  size_t dirs;

} file_dir_pair;


//expects last argument to be NULL
void load_file_dirs(char * dirs[]);

//returns a pointer to the next pair available, user must check if it has 2 dirs, or 1 only(only needed when last node is requested and get count returns an odd value)
file_dir_pair * get_next_pair();

//returns how many pairs are loaded
size_t get_pairs_count();

//free TAD memory, USER MUST always free the TAD when finished using them
void free_TAD();


//master llena el TAD, crea esclavos(execve), el esclavo le pide al TAD un par a procesar, hasta q no haya mas pares disponibles
