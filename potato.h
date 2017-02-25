#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <memory.h>
#include <ctype.h>
#include <time.h>

#define BUFF_LEN   512
#define MAX_POTATO 512


#define S "start"
#define P "potato"
#define E "end"
//#define r "ready"
//#define t "terminate"

typedef struct potato {
  char msg_type;
  /* enum msg_type_t{ */
  /*   S,  */
  /*   P,  */
  /*   E, */
  /* }msg_type; */
  int total_hops;
  int hop_count;
  int total_player;
  unsigned long hop_trace[MAX_POTATO];
} POTATO_T;


void clear_string( char *s ) {
  int i;
  for ( i=0; i < BUFF_LEN; i++ ) {
    s[i] = '\0';
  }
}

char* skipWs(char* line){
  while(*line != '\0' && isspace(*line)){
    line++;
  }
  return line;
}

/*see if the input is a long int or not, but will change the pointer's address*/
int readInt(char** linep, int* out){
  char* temp;
  *out = strtol(*linep, &temp, 0);
  if(*linep == temp){
    return 0; //could not read an int
  }
  *linep = temp; //update line pointer
  return 1; //succeeded
}

int selectfd(int fd1, int fd2, int fd3){
  fd_set fds;
  int maxfd;
  int res;
  char buf[BUFF_LEN];

  FD_ZERO(&fds);
  FD_SET(fd1, &fds);
  FD_SET(fd2, &fds);
  FD_SET(fd3, &fds);

  int max1 = fd1 > fd2 ? fd1:fd2;
  maxfd = fd3 > max1? fd3:max1;

  select(maxfd + 1, &fds, NULL, NULL, NULL);
  if(FD_ISSET(fd1, &fds)){
    return fd1;
  }
  if(FD_ISSET(fd2, &fds)){
    return fd2;
  }
  if(FD_ISSET(fd3, &fds)){
    return fd3;
  }
}

int selectAll(int fd_arr[], int size){
  fd_set fds;
  int maxfd;
  int res;
  char buf[BUFF_LEN];

  FD_ZERO(&fds);
  maxfd = fd_arr[0];
  for(int i = 0; i < size; i++){
    FD_SET(fd_arr[i], &fds);
    if(fd_arr[i] > maxfd){
      maxfd = fd_arr[i];
    }
  }
  select(1024, &fds, NULL, NULL, NULL);

  for(int i = 0; i < size; i++){
    if(FD_ISSET(fd_arr[i], &fds)){
      //return fd2;
      res = fd_arr[i]; //or just return?
      return res;
    }
    continue;
  }
  return res;
}
