// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include "bufferTAD.h"

#define MAX 256
#define NAME_SIZE 20
ssize_t getline(char **lineptr, size_t *n, FILE *stream);

int main(int argc, char * argv[]){
    char shm_name[NAME_SIZE+1];

    switch(argc){
        case 1:{ //gets the shm name through pipe read-end || STDIN_FILENO
            int bytes_read=read(STDIN_FILENO, shm_name, NAME_SIZE);
            if(bytes_read == -1){
              perror("couldn't read name");
              exit(1);
            }
            shm_name[bytes_read]='\0';
            break;
        }
        case 2:{ //gets the shm name through an argument when exec'd in other tty or foreground
            strncpy(shm_name, argv[1], NAME_SIZE);
            break;
        }
        default:{
            perror("Cantidad incorrecta de argumentos");
            exit(1);
        }
    }  

    shm_data buff = start_shm(shm_name,PROT_READ);
    buffer_open(buff);
    buffer_map(buff,PROT_READ);

    int max_files = read_max_files(buff);
    printf("max files: %d\n", max_files);
    char out[MAX];
    while(max_files > 0){
        printf("current max files: %d\n", max_files);
        memset(out,0,MAX);
        read_buff(buff,out);
        printf("%s\n",out);
        max_files--;
    }

    buffer_close(buff);
    buffer_unlink(buff);
    buffer_free(buff);
    return 0;
}
