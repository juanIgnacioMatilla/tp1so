#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/select.h>
#include <stdlib.h>
#include <sys/wait.h>

#define SLAVE_MAX 5
#define BLOCK 5


int pipes_array[SLAVE_MAX][2];


char * int_to_str(int src);

int main(int argc, char *argv[]){
  errno = 0;
  if(argc == 1){
    printf("Cantidad incorrecta de argumentos\n");
    return 0;
  }

  //crear slaves, y abrir los pipes
  size_t slaves = (argc) / 2 > SLAVE_MAX? SLAVE_MAX:(argc)/2;
  printf("slaves created: %zu\n", slaves);
  for(size_t i = 0; i < slaves; i++) {
    if(pipe(pipes_array[i]) != 0) {
      perror("couldn't create pipe, exiting");
      return errno;
    }
    printf("creating pipe nro: %zu\n", i+1);
    if(fork() != 0) {
      //parent process
      close(pipes_array[i][0]);
    } else {

      //child process
      printf("child nro: %zu\n", i+1);
      close(pipes_array[i][1]);
      char* pipefd = int_to_str(pipes_array[i][0]);
      if(pipefd == NULL) {
        return errno;
      }

      printf("pipes fd made str, now will execve to slave\n");
      execve("slave.out", &pipefd, NULL);
    }
  }
  

  size_t array_index = 0;
  fd_set rfds[slaves];
  for(;array_index < slaves; array_index++) {
    FD_ZERO(&rfds[array_index]);
    FD_SET(pipes_array[array_index][0], &rfds[array_index]);
  }
  printf("setted FD sets, now commencig to send files dirs\n");
  array_index = 0;
  int result = 0;
  unsigned short writes = 0;
  size_t argv_index = 0;
  while(argv_index < argc) {
    int ready = 0;
    while (!ready) {
      FD_ZERO(&rfds[array_index]);
      FD_SET(pipes_array[array_index][0], &rfds[array_index]);
      result = select(pipes_array[array_index][0] + 1, &rfds[array_index],NULL, NULL, NULL);
      if (result < 0) {
        perror("select failed");
        return 1;
      }
      else if (result == 0) {
        continue;
      }
      else if (FD_ISSET(pipes_array[array_index][0], &rfds[array_index])) {
        ready = 1;
      }
    }
    write(pipes_array[array_index][1], argv[argv_index], strlen(argv[argv_index]));
    if(writes == 2) {
      writes++;
      array_index++;
    }
    array_index = (array_index + 1) % slaves;
  }
  array_index = 0;
  for(;array_index < slaves;array_index++)
    close(pipes_array[array_index][1]);

  array_index = 0;
  for(;array_index < slaves; array_index++)
    wait(NULL);
  return 0;
}




char * int_to_str(int integer) {
  char * pipefd = malloc(sizeof(char)*BLOCK);
  int blocks = 1;
  int copied_digits = 0;
  if(pipefd == NULL) {
    printf("failed while allocating mem to transfer fd value");
    return NULL;
  }
  pipefd[copied_digits] = '0';
  while(integer != 0) {
    if(copied_digits == blocks * BLOCK) {
      pipefd = realloc(pipefd, blocks * BLOCK + BLOCK);
      if(pipefd == NULL) {
        printf("failed while allocating mem to transfer fd value");
        return NULL;
      }
    }
    pipefd[copied_digits++] = integer % 10 + '0';
    integer/=10;
  }
  pipefd = realloc(pipefd, copied_digits + 1);
  pipefd[copied_digits] = '\0';
  return pipefd;
}
