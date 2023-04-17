#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include "bufferTAD.h"


int main(int argc, char * argv[]){
    if(argc-1 != 3){
        perror("missing shared mem info\n");
        return 1;
    }
    shm_data buff = 
    // shm_data buff = start_shm(S_IRUSR);
    // buffer_open(buff);
    // buffer_map(buff,PROT_READ);
    printf("%d\n",argc);
    int i = 0;
    while(argv[i] != NULL){
        printf("%s\n",argv[i++]);
    }
    return 0;
}