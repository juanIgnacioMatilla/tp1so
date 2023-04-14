#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define BUFFER_SIZE 100

int pclose(FILE *stream);
FILE *popen(const char *command, const char *type);
ssize_t getline(char **lineptr, size_t *n, FILE *stream);


int main(void){ 
    errno = 0;
    char * filename = NULL;
    char buffer[BUFFER_SIZE] = {0};
    int bytes_read = 0;
    size_t len = 0;
    FILE * output;
    while((bytes_read = getline( &filename, &len, STDIN_FILENO)) > -1) {
        filename[bytes_read - 1] = '\0';
        sprintf(buffer, "md5sum %s", filename);
        output = popen(buffer, "r");
        fread(buffer, sizeof(char), BUFFER_SIZE, output);
        buffer[33] = '\0';
        write(STDOUT_FILENO, buffer, strlen(buffer));
        pclose(output);
        free(filename);
    }

    return 0;
}