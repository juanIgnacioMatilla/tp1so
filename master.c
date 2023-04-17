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
#include "bufferTAD.h"
#define SLAVE_MAX 5
#define PERROR_ROUTINE(msg, value) {perror(msg);return value;}
#define READ_END 0
#define WRITE_END 1
#define BUFFER_SIZE 256


typedef struct {
  int master_to_slave_pfd[2];
  int slave_to_master_pfd[2];
  int pid;
}slave_resources;

typedef slave_resources slave_resources_array[SLAVE_MAX];
slave_resources_array slave_array;

int slaves;

fd_set read_set;

int setup_slaves(int args);
int setup_child_pipes(int index);
void on_exit_routine(int index);
void close_master_pipes();

int run_tasks(int file_count, char * files[]);
int master_read(int * files_received);
int deliver_task_pid();
void deliver_all_once();
int deliver_task_pid(int slave_pid, char * filename);

slave_resources * get_slave(int slave_pid);

shm_data sh_mem;
FILE *fp;
int main(int argc, char *argv[]) {
  errno = 0;
  if(argc == 1) {
    perror("Cantidad incorrecta de argumentos\n");
    return 0;
  }
  sh_mem = start_shm("buffer",PROT_WRITE);
  buffer_open(sh_mem);
  buffer_map(sh_mem,PROT_WRITE);
  fp = fopen("output.txt","a+");
  load_max_files(sh_mem,argc-1);
  //setvbuf(stdout, NULL, _IONBF,0);
  print_data(sh_mem);
  sleep(2);
  int err_value;
  if((err_value = setup_slaves(argc)) == -1)
    PERROR_ROUTINE("Couldn't create slaves", errno);
  err_value = 0;
  run_tasks(argc-1, &argv[1]);
  on_exit_routine(slaves);
  fclose(fp);
  return 0;
}


int run_tasks(int file_count,char * files[]) {
  int files_delivered = 0; //files delivered al slave
  int files_received = 0; //files recieved from slave
  int closed_pipes = 0;
  ssize_t err_value;
  int slave_pid;
  

  deliver_all_once(files);
  files_delivered = slaves;
  while(files_received < file_count) {
    if(files_received == 1){
      sleep(2);
    }
    if((slave_pid = master_read(&files_received)) < 0)
      PERROR_ROUTINE("failed when reading from slaves", -1);
    if(slave_pid != 0){
      if(files_delivered < file_count) { //chequear que haya un slave pid para agregar, osea que haya terminado al menos 1 slave
        if((err_value=deliver_task_pid(slave_pid, files[files_delivered])) != 1)
          PERROR_ROUTINE("failed while giving more tasks to slave", -1);
          files_delivered++;
      }
      if(files_delivered == file_count && closed_pipes == 0) {
        close_master_pipes();
        closed_pipes = 1;
      }
    }
  }
  return 0;
}

//le da un nuevo trabajo al slave
int deliver_task_pid(int slave_pid, char * filename) {
  errno = 0;
  int err_value;
  char buffer[BUFFER_SIZE];
  slave_resources * slave = get_slave(slave_pid);
  if(slave == NULL)
    PERROR_ROUTINE("slave not found", -1);
  strcpy(buffer, filename);
  buffer[strlen(filename)] = '\n';
  // buffer[strlen(filename)+1] = '\0';
  if((err_value = write(slave->master_to_slave_pfd[WRITE_END], buffer, strlen(filename)+1) < 0) && errno != 0)
    PERROR_ROUTINE("failed to write on pipe master to slave", -1);
  return 1;
}

slave_resources * get_slave(int slave_pid) {
  for(int i = 0; i < slaves; i++) {
    if(slave_array[i].pid == slave_pid)
      return &slave_array[i];
  }
  return NULL;
}

void deliver_all_once(char * files[]) {
  for(int i = 0; i < slaves;i++) {
    deliver_task_pid(slave_array[i].pid, files[i]);
  }
  return;
}


int master_read(int * files_received) {
  int i = 0;
  int fd;
  int slave_pid = 0;
  int fd_max = -1;
  FD_ZERO(&read_set);
  
  for(; i < slaves;i++) {
    fd = slave_array[i].slave_to_master_pfd[READ_END];
    fd_max = fd_max < fd?fd:fd_max;
    FD_SET(fd, &read_set);
  }
  char buffer[BUFFER_SIZE] = {0};
  fd_max+=1; //valor maximo de fd que tenemos
  if(select(fd_max, &read_set, NULL, NULL, NULL) < 0) { //select se fija si hay algun fd listo para el recibir un resultado
    PERROR_ROUTINE("failed select", -1);
  } else {
    for(int i = 0; i<slaves;i++) {
      fd = slave_array[i].slave_to_master_pfd[READ_END];
      if(FD_ISSET(fd, &read_set)) { //chequeo si este fd es el que esta listo para recibir el resultado
        slave_pid = slave_array[i].pid;
        int bytes_read = read(fd, buffer, BUFFER_SIZE);
        if(bytes_read == -1)
          PERROR_ROUTINE("failed while reading from slave", -1);
        buffer[bytes_read] = '\0';
        fwrite(buffer , 1 , strlen(buffer) , fp );
        load_buff(sh_mem,buffer);
        (*files_received)++;
        return slave_pid;
      }
    }
  }
  for(; i < slaves;i++) {
    fd = slave_array[i].slave_to_master_pfd[READ_END];
    FD_CLR(fd, &read_set);
  }
  
  return slave_pid;
}

void close_master_pipes() {
  int i = 0;
  int err_value = 0;
  for(; i < slaves; i++) {
     
    if((err_value = close(slave_array[i].master_to_slave_pfd[WRITE_END])) == -1){
      perror("couldn't close master to slave pipe write-end");
      return;
    }
  }
}


void on_exit_routine(int slave_count) {
  int err_value = 0;

  int status;
  int i = 0;
  for(; i < slave_count; i++) {
    while(waitpid(slave_array[i].pid, &status, 0) < 0)  

    if(status != 0) {
      printf("child with errors: %d\n", slave_array[i].pid);
      perror("statuscode error when waiting for child");
    }
    if((err_value = close(slave_array[i].slave_to_master_pfd[READ_END])) == -1){
      perror("couldn't close slave to master pipe read-end");
      return;
    }
  }
  // int stat;
  // do
  // {
  //   if((stat = check_shm_status()) == 0)
  //     close_shm();
  // } while (stat != 0 && sleep(1));
  return;
}


int setup_child_pipes(int index) {
  int err_value = 0;
  //close pipe ends that belong to master
  if((err_value = close(slave_array[index].master_to_slave_pfd[WRITE_END])) == -1)
    PERROR_ROUTINE("couldn't close master to slave pipe read-end", -1);
  if((err_value = close(slave_array[index].slave_to_master_pfd[READ_END])) == -1)
    PERROR_ROUTINE("couldn't close slave to master pipe read-end", -1);

  
  //setup new stdin and stdout for slaves
  if((err_value = dup2(slave_array[index].master_to_slave_pfd[READ_END], STDIN_FILENO)) == -1)
    PERROR_ROUTINE("couldn't dup2 of master to slave read-end with stdin", -1);
  if((err_value = dup2(slave_array[index].slave_to_master_pfd[WRITE_END], STDOUT_FILENO)) == -1)
    PERROR_ROUTINE("couldn't dup2 of slave to master write-end with stdin", -1);

  //close for child it's pipes fd since they are now the stdin and stdout
  if((err_value =  close(slave_array[index].master_to_slave_pfd[READ_END])) == -1)
    PERROR_ROUTINE("couldn't close master to slave pipe read-end", -1);
  if((err_value =  close(slave_array[index].slave_to_master_pfd[WRITE_END])) == -1)
    PERROR_ROUTINE("couldn't close slave to master write-end", -1);

  return 0;
}


int setup_slaves(int args) {
  int err_value = 0;
  char * argv[] = {"slave", NULL}; //crea argumentos
  slaves = (args) / 2 > SLAVE_MAX? SLAVE_MAX:(args)/2;  //calculo de cantidad de slaves
  int index = 0;
  for(; index < slaves;index++) { //loop para crear pipes mts y stm
      if((err_value = pipe(slave_array[index].master_to_slave_pfd)) != 0)
        PERROR_ROUTINE("master to slave pipe creation failed", -1);
      if((err_value = pipe(slave_array[index].slave_to_master_pfd)) != 0)
        PERROR_ROUTINE("slave to master pipe creation failed", -1);

      int pid = fork();
      if(pid == -1) {
        PERROR_ROUTINE("fork failed", -1);
      } else if( pid != 0) {
        //parent process
        slave_array[index].pid = pid;
        
        if((err_value = close(slave_array[index].master_to_slave_pfd[READ_END])) == -1) //slaves usa estos pipes entonces como no sirven los cerramos.
          PERROR_ROUTINE("couldn't close master to slave pipe read-end", -1);

        if((err_value = close(slave_array[index].slave_to_master_pfd[WRITE_END])) == -1)
          PERROR_ROUTINE("couldn't close master to slave pipe read-end", -1);
        
      } else {
        //child process
        if((err_value = setup_child_pipes(index)) != 0)
          PERROR_ROUTINE("error on setup child process pipe", -1);
          execv(argv[0], argv);
      }
  }
  return index;
}