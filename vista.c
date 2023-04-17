#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include "bufferTAD.h"

#define MAX 512
ssize_t getline(char **lineptr, size_t *n, FILE *stream);

int main(int argc, char * argv[]){
    char * name;
    size_t len;
    int bytes_read;
    if(argc == 2){
        int aux = strlen(argv[2]);
        strncpy(name,argv[2],aux);
        name[aux-1] = '\0';
    }
    else if((bytes_read = getline(&name,&len,stdin)) != -1){
        perror("No shared mem given\n");
        exit(1);
    }
    
    shm_data buff = start_shm(argv[2],PROT_READ);
    buffer_open(buff);
    buffer_map(buff,PROT_READ);

    printf("te estas portando mal\n");
    int max_files = read_max_files(buff);
    char out[MAX];
    while(max_files > 0){
        memset(out,0,MAX);
        read_buff(buff,out);
        printf("%s\n",out);
        max_files--;
    }
    buffer_close(buff);
    buffer_free(buff);
    return 0;
}