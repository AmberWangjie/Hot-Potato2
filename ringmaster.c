#include "potato.h"

POTATO_T* s_potato;

int makeFifo(int p_num){
  int make = 0;
  for(int i = 0; i < p_num; i++){
    char mp[BUFF_LEN];
    char pm[BUFF_LEN];
    char ppr[BUFF_LEN];
    char ppl[BUFF_LEN];
    memset(mp, '\0', BUFF_LEN);
    memset(pm, '\0', BUFF_LEN);
    memset(ppr, '\0', BUFF_LEN);
    memset(ppl, '\0', BUFF_LEN);
    sprintf(mp, "/home/bitnami/650/hw2/hw2/tmp/master_p%d", i);
    sprintf(pm, "/home/bitnami/650/hw2/hw2/tmp/p%d_master", i);
    if(i == p_num - 1){
      sprintf(ppr, "/home/bitnami/650/hw2/hw2/tmp/p%d_p%d", i,0);
      sprintf(ppl, "/home/bitnami/650/hw2/hw2/tmp/p%d_p%d", 0,i);
    }else{
      sprintf(ppr, "/home/bitnami/650/hw2/hw2/tmp/p%d_p%d", i,i+1);
      sprintf(ppl, "/home/bitnami/650/hw2/hw2/tmp/p%d_p%d", i+1,i);
    }
    if(mkfifo(mp, 0664) == -1){
      perror("mkfifo from master to player failed\n");
      printf("%d: %s\n", errno, strerror(errno));
      make = 0;
      break;
    }else{
      //printf("DEBUG:successfully make fifo from master to player\n");
      make = 1;
    }
    if(mkfifo(pm, 0664) == -1){
      perror("mkfifo from player to master failed\n");
      printf("%d: %s\n", errno, strerror(errno));
      make = 0;
      break;
    }else{
      // printf("DEBUG:successfully make fifo from player to master\n");
      make = 1;
    }
    if(mkfifo(ppr, 0664) == -1){
      perror("mkfifo from player to its right neighbor failed\n");
      printf("%d: %s\n", errno, strerror(errno));
      make = 0;
      break;
    }else{
      //printf("DEBUG:successfully make fifo from player to its right neighbor\n");
      make = 1;
    }
    if(mkfifo(ppl, 0664) == -1){
      perror("mkfifo from player to its left neighbor failed\n");
      printf("%d: %s\n", errno, strerror(errno));
      make = 0;
      break;
    }else{
      //printf("DEBUG:successfully make fifo from player to its left neighbor\n");
      make = 1;
    }
  }
  return make;
}

int rmFifo(int p_num){
  int link = 0;
  for(int i = 0; i < p_num; i++){
    char mp[BUFF_LEN];
    char pm[BUFF_LEN];
    char ppr[BUFF_LEN];
    char ppl[BUFF_LEN];
    memset(mp, '\0', BUFF_LEN);
    memset(pm, '\0', BUFF_LEN);
    memset(ppr, '\0', BUFF_LEN);
    memset(ppl, '\0', BUFF_LEN);
    sprintf(mp, "/home/bitnami/650/hw2/hw2/tmp/master_p%d", i);
    sprintf(pm, "/home/bitnami/650/hw2/hw2/tmp/p%d_master", i);

    if(i == p_num - 1){
      sprintf(ppr, "/home/bitnami/650/hw2/hw2/tmp/p%d_p%d", i,0);
      sprintf(ppl, "/home/bitnami/650/hw2/hw2/tmp/p%d_p%d", 0,i);
    }else{
      sprintf(ppr, "/home/bitnami/650/hw2/hw2/tmp/p%d_p%d", i,i+1);
      sprintf(ppl, "/home/bitnami/650/hw2/hw2/tmp/p%d_p%d", i+1,i);
    }

    unlink(mp);
    unlink(pm);
    unlink(ppr);
    unlink(ppl);
  }
  return link;
}

int masterExecute(int p_num, int h_num){
  /*intialize the start_potato*\/ */
  //printf("DEBUG:m_e initial\n");
  int m_state = 1; /*if bad happend*/
  s_potato = malloc(sizeof(POTATO_T));
  /*print intial parms*/
  fprintf(stdout, "Potato Ringmaster\nPlayers = %d\nHops = %d\n", p_num, h_num);
  /*send start msg to players*/
  s_potato->msg_type = 'S';
  s_potato->total_hops = h_num;
  s_potato->total_player = p_num;
  //  s_potato->hop_count = s_potato->total_hops - 1; /*verify this*/
  /*open all the m_p and p_m fifo*/
  int mp_arr[p_num] ;
  int pm_arr[p_num] ;
  for(int i = 0; i < p_num; i++){
    char mp[BUFF_LEN];
    char pm[BUFF_LEN];
    memset(mp, '\0', BUFF_LEN);
    memset(pm, '\0', BUFF_LEN);
    sprintf(mp, "/home/bitnami/650/hw2/hw2/tmp/master_p%d", i);
    sprintf(pm, "/home/bitnami/650/hw2/hw2/tmp/p%d_master", i);
    mp_arr[i] = open(mp, O_WRONLY);
    pm_arr[i] = open(pm, O_RDONLY);
  }//end of open for
  for(int i = 0; i < p_num; i++){
    write(mp_arr[i], s_potato, sizeof(*s_potato));
    read(pm_arr[i], s_potato, sizeof(*s_potato));
    //if(s_potato->msg_type == 'r'){
    fprintf(stdout, "Player %d is ready to play\n", i);
      //s_potato->msg_type == 'S';
      // }
  }
  sleep(5);
  /*change the msg to P and choose a player randomly*/
  s_potato->msg_type = 'P';
  s_potato->hop_count = s_potato->total_hops;
  srand((unsigned int)time(NULL));
  int random = rand()%p_num;
  fprintf(stdout, "All players present, sending potato to player %d\n", random);
  write(mp_arr[random], s_potato, sizeof(*s_potato));
  
  /*select the return player*/
  //  printf("DEBUG: master select all:\n");
  int it_fd = selectAll(pm_arr, p_num);
  read(it_fd, s_potato, sizeof(*s_potato));

  /*read success, print the trace and write end msg*/

  //printf("DEBUG: master print the trace(total is %d): \n", s_potato->total_hops);
  fprintf(stdout, "Trace of potato:\n");

  for(int i = 0; i < s_potato->total_hops; i++){
    if(i == s_potato->total_hops - 1){
      fprintf(stdout, " %lu\n", s_potato->hop_trace[i]);
    }else{
      fprintf(stdout, "%lu, ", s_potato->hop_trace[i]);
    }
  }
  //  s_potato->msg_type = 'E';
  //  s_potato->total_hops = h_num;
  
  for(int i = 0; i < p_num; i++){
    s_potato->msg_type = 'E';
    write(mp_arr[i], s_potato, sizeof(*s_potato));
  }
 
  for(int i = 0;i < p_num; i++) {
    read(pm_arr[i], s_potato, sizeof(POTATO_T));
    //printf("DEBUG: player %d is ready to terminate\n", i); 
  }
  //  read(pm_arr[i], s_potato, sizeof(*s_potato));
  /*close all the fd*/
  //printf("DEBUG: master close:\n");
  close(it_fd);
  //  sleep(5);
  for(int i = 0; i < p_num; i++){
    close(pm_arr[i]);
    close(mp_arr[i]);
  }
  //sleep(5);
  // printf("DEBUG: master unlink: \n");
  rmFifo(p_num);
  //  free(s_potato);
  return m_state;
}


int main(int argc, char *argv[]){
  int count = 0;
  if(argc != 3){
    fprintf(stderr, "number of arguments is not right!\nUseage: ringmaster <number_of_players> <number_of_hops>\n");
    return EXIT_FAILURE;
  }
  int arg_p;
  int arg_h;
  char* ptr1 = strchr(argv[0],'\0');
  argv[1] = skipWs(ptr1+1);
  char* arg1 = argv[1];
  char* ptr2 = strchr(argv[1],'\0');
  argv[2] = skipWs(ptr2+1);
  char* arg2 = argv[2];
  if(!readInt(&arg1,&arg_p) || !readInt(&arg2,&arg_h)){
      fprintf(stderr, "Second and third argument should be an integer!\n");
      return EXIT_FAILURE;
  }
  else if(atoi(argv[1]) <= 1 || atoi(argv[2]) < 0 || atoi(argv[2]) >= MAX_POTATO){
    fprintf(stderr, "number of potatoes is too small or number of hops is not in the range!\n");
    return EXIT_FAILURE;
  }
  /*how to handle the number with digit after point?*/
  else{
    //  printf("DEBUG: Now the input should be valid,\n");
    if(!makeFifo(atoi(argv[1]))){
      return EXIT_FAILURE;
    }
    if(!masterExecute(atoi(argv[1]), atoi(argv[2]))){
      return EXIT_FAILURE;
    }
  }
 
  return EXIT_SUCCESS;
}
 
