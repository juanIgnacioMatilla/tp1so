#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define SLAVE_MAX 5

//recibe los archivos para encriptar
//distribuye entre los esclavos y les da mas archivos si es que faltan ni bien terminan
//recibe el resultado de los esclavos y lo carga en un buffer por orden de llegada
//le entrega el buffer al proceso vista
//guarda todo el output en un archivo resultado

int main(int argc, char *argv[]){
    if(argc == 1){
        printf("Cantidad incorrecta de argumentos\n");
        return 0;
    }

    int argCant = argc -1;
    for(int i = 1; i < argCant; i++){
        DIR * directory = argv[i];

    }
}   
