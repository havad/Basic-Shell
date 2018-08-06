#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "mycd.h"
#include "pwd.h"

//this function creates an array of strings to pass to the child process using execvp
char** execPrep(char* cmd){
  char** result = NULL;
  char* next = strtok(cmd, " ");
  int n_spaces = 0;

  while(next){
    n_spaces++;
    result = realloc(result, sizeof(char*) * n_spaces);

    if(result == NULL)
      exit(-1);

    result[n_spaces - 1] = next;
    next = strtok(NULL, " ");
  }

  result = realloc(result, sizeof(char*) * (n_spaces + 1));
  result[n_spaces] = NULL;

  return result;
}

//this function creates an array of strings to pass to the child process using execvp
char** pipe1Prep(char* cmd){
  char** result = NULL;
  char* next = strtok(cmd, " ");
  int n_spaces = 0;

  while(next){
    n_spaces++;
    result = realloc(result, sizeof(char*) * n_spaces);

    if(strncmp(next, "|", strlen("|")) == 0){
      result[n_spaces - 1] = NULL;
      return result;
    }

    if(result == NULL)
      exit(-1);

    result[n_spaces - 1] = next;
    next = strtok(NULL, " ");
  }

  result = realloc(result, sizeof(char*) * (n_spaces + 1));
  result[n_spaces] = NULL;

  return result;
}

//this function creates an array of strings to pass to the child process using execvp
char** pipe2Prep(char* cmd){
  char** result = NULL;
  char* next = strtok(cmd, " ");
  int n_spaces = 0, nextcmd = 0;

  while(next){
    if(nextcmd){
      n_spaces++;
      result = realloc(result, sizeof(char*) * n_spaces);

      if(result == NULL){
        exit(-1);
      }

      result[n_spaces - 1] = next;
    }

    if(strncmp(next, "|", strlen("|")) == 0){
      nextcmd = 1;
    }

    next = strtok(NULL, " ");
  }

  n_spaces += 2;
  result = realloc(result, sizeof(char*) * (n_spaces));
  result[n_spaces - 2] = "pipebuf";
  result[n_spaces - 1] = NULL;

  return result;
}


int main(int argc, char** argv){
  char* command = NULL;
  size_t len;

  printf("mysh$ ");
  int ret = getline(&command, &len, stdin);//read first command
  char cwd[1024];
  if(getcwd(cwd, sizeof(cwd)) != NULL){
    char* path = getenv("PATH");
    if(!strstr(path, cwd)){
      char* temp = malloc((strlen("PATH=") + strlen(path) + strlen(":") + strlen(cwd) + 1) * sizeof(char));
      strncpy(temp, "PATH=", strlen("PATH=") + 1);
      strncat(temp, path, strlen(path));
      strncat(temp, ":", strlen(":"));
      strncat(temp, cwd, strlen(cwd));
      putenv(temp);
      free(temp);
    }
  }else{
    printf("Error getting current directory to add to path...\n");
    exit(-1);
  }

  while(strncmp(command, "exit", strlen("exit")) != 0){//check if exit
    if(ret == EOF){//check for error from getline
      printf("Error reading command... try again\n");
    }


    command[strlen(command) - 1] = '\0';//overwrite the newline character with null terminator

    int pipefd[2];
    int p = 0;
    if(strstr(command, "|") != NULL){//create the pipe handles if necessary
      if(pipe(pipefd) == -1){
        printf("Error creating pipe...\n");
        exit(-1);
      }
      p = 1;
    }

    if(strncmp(command, "mycd", strlen("mycd")) == 0){//check if the command is mycd
      if(strlen(command) > strlen("mycd")){
        char* dir = strrchr(command, ' ') + 1;//gets the string following the last space in the command
        mycd(dir);
      }else{
        printf("Supply a directory for mycd\n");
      }
    }else if(strncmp(command, "pwd", strlen("pwd")) == 0){//check if the command is pwd
      pwd();
    }else{//do a non built in command
      if(p){
        pid_t pid1 = fork();
        if(pid1 == 0){
          close(pipefd[0]);
          dup2(pipefd[1], 1);
          close(pipefd[1]);
          char** result = pipe1Prep(command);
          if(execvp(result[0], result) == -1){
            printf("Error in first pipe command...\n");
            exit(-1);
          }
        }else if(pid1 == -1){
          printf("Error forking pipe child process 1...\n");
        }else{
        }
        pid_t pid2 = fork();
        if(pid2 == 0){
          close(pipefd[1]);
          char in;
          int fd = open("pipebuf", O_WRONLY|O_TRUNC|O_CREAT, 0666);
          while(read(pipefd[0], &in, 1) > 0){
            write(fd, &in, 1);
          }
          close(fd);
          close(pipefd[0]);
          char** result = pipe2Prep(command);
          if(execvp(result[0], result) == -1){
            printf("Error in second pipe command...\n");
            exit(-1);
          }
        }else if(pid2 == -1){
          printf("Error forking pipe child process 2...\n");
        }else{
        }
        close(pipefd[0]);
        close(pipefd[1]);
        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);
        remove("pipebuf");
      }else{
        pid_t pid = fork();
        if(pid == 0){
          char** result = execPrep(command);

          int i = 0;
          while(result[i] != NULL){
            
            if(( strstr(result[i], "<")) != NULL){
              //redirect input
              char* temp = malloc(strlen(result[i]) * sizeof(char));
              int j = 0, k = 0;
              while(result[i][j] != '\0'){
                if(result[i][j] != '<'){
                  temp[k] = result[i][j];
                  k++;
                }
                j++;
              }
              temp[k] = '\0';
              result[i] = temp;
              int fd = open(result[i], O_RDONLY);
              dup2(fd, 0);
              close(fd);
            }
            if(( strstr(result[i], ">")) != NULL){
              //redirect output
              char* temp = malloc(strlen(result[i]) * sizeof(char));
              int j = 0, k = 0;
              while(result[i][j] != '\0'){
                if(result[i][j] != '>'){
                  temp[k] = result[i][j];
                  k++;
                }
                j++;
              }
              temp[k] = '\0';
              result[i] = temp;
              int fd = open(result[i], O_WRONLY|O_TRUNC|O_CREAT, 0666);
              dup2(fd, 1);
              close(fd);
            }
            i++;
          }
          if(execvp(result[0], result) == -1){
            printf("Unknown command... try again, or use 'exit' to quit\n");
            exit(-1);
          }
        }else if(pid == -1){
          printf("Error forking child process...\n");
        }else{
          wait(NULL);
        }
      }
    }

    printf("mysh$ ");
    ret = getline(&command, &len, stdin);//get next command
  }
  free(command);
  command = NULL;

  return 0;
}
