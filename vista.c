#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include "bufferTAD.h"


int main(int argc, char * argv[]){
    const char * name = "buffer";
    const char * sem_1 = "semaforo1";
    const int struct_size = sizeof(result_data);

    int check = buffer_open(name, O_RDONLY , 0666);
    if(check == -1){
        perror("There was an error opening the shared memory");
        exit(1);
    }

    //ver como recibimos la cantidad de archivos procesados para imprimir
    buffer_down(); //espera a que este en 0, lo pone en -1 para que nadie interfiera
    
    result_data * file_ptr = buffer_map(NULL, struct_size, PROT_READ, MAP_SHARED, 0);

    if(file_ptr == MAP_FAILED){
        perror("Threre was an error accessing the shared memory");
        exit(1);
    }
    //printeo por stdout de lo que hay en la shared mem
    printf("Nombre: %s\n",file_ptr->name);
    printf("md5: %d\n",file_ptr->md5);
    printf("ID: %d\n",file_ptr->pid);
    printf("\n");

    buffer_up();

    buffer_unmap(file_ptr,struct_size);
    buffer_close();

    return 0;

}