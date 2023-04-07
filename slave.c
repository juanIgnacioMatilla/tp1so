#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <openssl/md5.h>
#include <pthread.h>
#include <stdlib.h>

pthread_mutex_t mutex;

#define BUFFER_SIZE 1024


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

int main(int argc, char * argv[]) {
  if(argc < 3)
    return 1;
  pthread_t thread1, thread2;
  pthread_create(&thread1, NULL, md5_calculate, (void *) argv[0]);
  pthread_create(&thread2, NULL, md5_calculate,  (void *) argv[1]);

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);

  return 0;
}
