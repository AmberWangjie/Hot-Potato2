#include "potato.h"

int masterExec(int p_num, int h_num){
  /*intialize the start_potato*/
  printf("DEBUG:m_e initial\n");
  int m_state = 0; /*if bad happend*/
  POTATO_T* s_potato;
  if((s_potato = malloc(sizeof(POTATO_T*))) == NULL){
    m_state = 0;
    fprintf(stderr, "malloc for POTATO_T failed!\n");
    free(s_potato);
    return m_state;
  }
  //if(s_potato->msg_type = malloc(strlen))
  s_potato->msg_type = 'S';
  s_potato->total_hops = h_num;
  s_potato->hop_count = 1; /*verify this*/
  //memset(s_potato->hop_trace,'\0', MAX_POTATO);
  //s_potato->hop_trace = {};/*how to initialize this array*/
  s_potato->hop_trace[0] = getpid();
  printf("DEBUG:m_e initial finished, the trace[0] is:%lu\n", s_potato->hop_trace[0]);
  /*create fifo for the selected player*/
  //for(int i = 0; i < p_num; p_num++){
    int fd;
    int fd_player;
    char m_name[128];
    memset(m_name, '\0',128);
    //char* pre = "/tmp/master_p";
    /*necessarg?*/
    /* char* append = itoa(i); */
    /* if((m_name = malloc(strlen(pre)+strlen(append)+1)) != NULL){ */
    /*   m_name[0] = '\0'; */
    /*   strcat(m_name, pre); */
    /*   strcat(m_name, append); */
    /*   printf("DEBUG: after append, the name string is:%s\n",name); */
    /* }else{ */
    /*   fprintf(stderr, "malloc for fifo name string failed!\n"); */
    /*   free(m_name); */
    /*   return -1; */
    /* } */
    srand((unsigned int)time(NULL));
    int i = rand()%(p_num - 1);
    /*why this cannot be written into tmp*/
    sprintf(m_name, "/home/bitnami/650/hw2/hw2/tmp/master_p%d", i);
    /*  FILE* fp = fopen("/home/bitnami/650/hw2/hw2/tmp", "w+");  */
    /*    if(fp != NULL){ */
    /* 	 fprintf(fp, "master_p%d", i); */
    /* /\*   fputs(m_name, fp); *\/ */
    /*    }else{ */
    /* 	 perror("write fifo name into dir failed\n"); */
    /* 	 printf("%d: %s\n", errno, strerror(errno)); */
    /* 	 m_state = 0; */
    /* 	 free(s_potato); */
    /* 	 return m_state; */
    /*    } */
    printf("DEBUG: The created fifo name is:%s\n", m_name);
    if(mkfifo(m_name, 0664) == -1){
      perror("mkfifo from master to player failed\n");
      printf("%d: %s\n", errno, strerror(errno));
      m_state = 0;
      free(s_potato);
      return m_state;;
    }else{
      printf("DEBUG:successfully make fifo from master to player\n");
      m_state = 1;
      /*now let's open it and do the real stuff*/
      //break;
      while(1){
	//char p_name[128];
	//memset(p_name, '\0',128);
	//sprintf(p_name, "/home/bitnami/650/hw2/hw2/tmp/p%d_master", i);
	if((fd_player = open(PLAYER_FIFO, O_WRONLY)) == -1){
	  perror("Failed to open server fifo");
	  printf("%d: %s\n", errno, strerror(errno));
	  m_state = -1;
	  break;
	}
	printf("DEBUG: has open the  player\n");
	char buf_msg[128];
	memset(buf_msg, '\0',128);
	strcpy(buf_msg, m_name);
	strcat(buf_msg, " ");
	strncat(buf_msg, &s_potato->msg_type,1);
	if(write(fd_player, buf_msg, strlen(buf_msg)) != strlen(buf_msg)){
	  perror("write to server");
	  printf("%d: %s\n", errno, strerror(errno));
	  break;
	}
	printf("DEBUG: has written to player\n");
      }
      if(unlink(m_name) == -1){
	perror("Failed to unlink the fifo:\n");
	printf("%d: %s\n", errno, strerror(errno));
	m_state = 0;
	free(s_potato);
	return m_state;
      }
      //      fclose(fp);
    }
  free(s_potato);
 
  return m_state;
}
//int count = 0;
//ringmaster
int main(int argc, char *argv[]){
  int count = 0;
  if(argc != 3){
    fprintf(stderr, "number of arguments is not right!\nUseage: ringmaster <number_of_players> <number_of_hops>\n");
    return EXIT_FAILURE;
  }
  int arg_p;
  int arg_h;
  printf("DEBUG: argv0 is:%s\n", argv[0]);
  char* ptr1 = strchr(argv[0],'\0');
  argv[1] = skipWs(ptr1+1);
  char* arg1 = argv[1];
  printf("DEBUG: argv1 is:%s, int is:%d\n", argv[1], atoi(argv[1]));
  char* ptr2 = strchr(argv[1],'\0');
  argv[2] = skipWs(ptr2+1);
  char* arg2 = argv[2];
  printf("DEBUG: argv2 is:%s, int is:%d\n", argv[2], atoi(argv[2]));
  if(!readInt(&arg1,&arg_p) || !readInt(&arg2,&arg_h)){
      fprintf(stderr, "Second and third argument should be an integer!\n");
      printf("DEBUG: err arg_p is:%s, int is:%d, err arg_h is:%s, int is:%d\n", arg1, atoi(arg1), arg2,atoi(arg2));
      return EXIT_FAILURE;
  }
  else if(atoi(argv[1]) <= 1 || atoi(argv[2]) < 0 || atoi(argv[2]) >= MAX_POTATO){
    fprintf(stderr, "number of potatoes is too small or number of hops is not in the range!\n");
    printf("DEBUG: err arg_p is:%s, int is:%d, err arg_h is:%s, int is:%d\n", argv[1], atoi(argv[1]), argv[2], atoi(argv[2]));
    return EXIT_FAILURE;
  }
  /*how to handle the number with digit after point?*/
  else{
    printf("DEBUG: Now the input should be valid\n");
    /*ringmaster execution process*/
    if(!masterExec(atoi(argv[1]), atoi(argv[2]))){
	fprintf(stderr,"Something bad happend when executing master program\n");
	return EXIT_FAILURE;
      }
  }
  //exit(0);
  return EXIT_SUCCESS;
}

