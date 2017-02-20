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

#define PLAYER_FIFO "/tmp/player_fifo"
#define S "start"
#define P "potato"
#define E "end"

typedef struct potato {
  char msg_type;
  /* enum msg_type_t{ */
  /*   S,  */
  /*   P,  */
  /*   E, */
  /* }msg_type; */
  int total_hops;
  int hop_count;
  
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
