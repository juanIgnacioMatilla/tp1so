#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define MAX_MD5_LENGTH 33
#define BUFFER_SIZE 1024

int pclose(FILE *stream);
FILE *popen(const char *command, const char *type);
ssize_t getline(char **lineptr, size_t *n, FILE *stream);

int get_md5(char * md5, char * filename);
char * format_output(char * md5, char * filename);


int main(void){ 
    errno = 0;
    char * filename = NULL;
    char md5[MAX_MD5_LENGTH] = {0};
    int bytes_read = 0;
    size_t len = 0;
    char * formated_md5;
    while((bytes_read = getline( &filename, &len, stdin)) > 0) {
        
        filename[bytes_read - 1] = '\0';
        if(get_md5(md5, filename) != 0) {
            perror("err while calculating md5");
            return errno;
        }
        
        md5[MAX_MD5_LENGTH - 1] = '\0';
        formated_md5 = format_output(md5, filename);
        
        write(STDOUT_FILENO, formated_md5, strlen(formated_md5));

        free(formated_md5);
        free(filename);
    }
    
    return 0;
}

char * format_output(char * md5, char * filename) {
    errno = 0;
    char * formated_output = malloc(sizeof(char) * BUFFER_SIZE);
    int size;
    if((size = sprintf(formated_output, "filename: %s\nmd5: %s\nslave_pid: %d", filename, md5, (int)getpid())) < 0) {
        perror("error while formatting md5");
        return NULL;
    }
    formated_output[size] = '\0';
    formated_output = realloc(formated_output,sizeof(char) * size);
    if(formated_output == NULL) {
        perror("couldn't realloc formated_output");
        return NULL;
    }
    return formated_output;
}


int get_md5(char * md5, char * filename) {
    errno = 0;
    char buffer[BUFFER_SIZE];
    FILE * output;
    if(sprintf(buffer, "md5sum %s", filename)<0) {
        perror("failed when copying to buffer");
        return errno;
    }

    output = popen(buffer, "r");
    if(output == NULL) {
        perror("couldn't open buffer");
        return errno;
    }

    if(fread(md5, sizeof(char), MAX_MD5_LENGTH, output) == 0) {
        perror("failed to get md5sum");
        return errno;
    }

    if(pclose(output) == -1){
        perror("failed to close buffer stream");
        return errno;
    }

    return 0;
}