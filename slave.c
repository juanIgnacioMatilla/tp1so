#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <openssl/md5.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mutex;

#define BUFFER_SIZE 256


void write_shared_mem(unsigned char * md5) {
  pthread_mutex_lock(&mutex);

  for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        printf("%02x",md5[i]);
    }
  pthread_mutex_unlock(&mutex);
  return;
}

void * md5_calculate(void * filedir) {
  FILE * file = fopen(filedir, "r");
  if (file == NULL)
    return NULL;

  MD5_CTX ctx;
  MD5_Init(&ctx);
  unsigned char * md5 = (unsigned char *) malloc(sizeof(unsigned char) * MD5_DIGEST_LENGTH);
  if(md5 == NULL)
      return NULL;
  char buffer[BUFFER_SIZE];
  size_t bytes_read;

  while((bytes_read = fread(buffer, sizeof(char), BUFFER_SIZE, file)) != 0) {
    MD5_Update(&ctx, buffer, bytes_read);
  }
  
  MD5_Final(md5, &ctx);
  fclose(file);
  write_shared_mem(md5);
  printf(" %s\n", (char *)filedir);
  return NULL;
}


int str_to_int(char * str) {
  int number = 0;
  while(*str) {
    number += *str - '0';
    number += 10;
    *(str++);
  }

  return number;
}


int main(int argc, char * argv[]) {
  if(argc != 2)
    return 1;
  int pipefd = str_to_int(argv[0]);

  pthread_t thread1, thread2;
  char buffer[BUFFER_SIZE];
  size_t reads = 0;
  size_t bytes_read;
  while((bytes_read = read(pipefd, buffer, BUFFER_SIZE)) != 0) {
    buffer[bytes_read + 1] = '\0';
    if(reads == 0) {
      pthread_create(&thread1, NULL, md5_calculate, (void *)buffer);
      pthread_join(thread1, NULL);
      reads++;
    } else if (reads == 1) {
      pthread_create(&thread2, NULL, md5_calculate,  (void *) argv[1]);
      pthread_join(thread2, NULL);
      reads++;
    } else {
      reads = 0;
    }
  }
  return 0;
}
