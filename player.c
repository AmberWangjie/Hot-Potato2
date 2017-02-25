

#include "potato.h"

POTATO_T* s_potato;

int max(int fd1, int fd2, int fd3){
  int max1 = fd1 > fd2 ? fd1:fd2;
  int res = fd3 > max1? fd3:max1;
  return res;
}

int playerExec(int p_id){
  int p_state = 1;
  /*initialize the potato struct*/
  s_potato = malloc(sizeof(POTATO_T));

  /*open all 6 fifos*/
  /*arr[0] is fd_send->wrt, arr[1] is fd_recv->rd*/
  char mp[BUFF_LEN];
  char pm[BUFF_LEN];
  char ppr_1[BUFF_LEN];
  char ppl_1[BUFF_LEN];
  char ppl_2[BUFF_LEN];
  char ppr_2[BUFF_LEN];
  memset(mp, '\0', BUFF_LEN);
  memset(pm, '\0', BUFF_LEN);
  memset(ppr_1, '\0', BUFF_LEN);
  memset(ppl_1, '\0', BUFF_LEN);
  memset(ppr_2, '\0', BUFF_LEN);
  memset(ppl_2, '\0', BUFF_LEN);
  sprintf(mp, "/home/bitnami/650/hw2/hw2/tmp/master_p%d", p_id);
  sprintf(pm, "/home/bitnami/650/hw2/hw2/tmp/p%d_master", p_id);

  int pm_arr[2];
  int ppr_arr[2];
  int ppl_arr[2];

  int p_num;
  int h_num;
  pm_arr[1] = open(mp, O_RDONLY);

  pm_arr[0] = open(pm, O_WRONLY);
  
  /*read the p_num from first read of master*/
  read(pm_arr[1], s_potato, sizeof(POTATO_T));
  if(s_potato->msg_type == 'S'){ 
    p_num = s_potato->total_player;
    h_num = s_potato->total_hops;
    fprintf(stdout, "Connected as player %d out of %d total players\n", p_id, p_num);
    // printf("get the h_num from potato: %d\n", s_potato->total_hops);
    //s_potato->msg_type = 'r'; 
    write(pm_arr[0], s_potato, sizeof(POTATO_T));
  }

  if(p_id == p_num - 1){
    sprintf(ppr_1, "/home/bitnami/650/hw2/hw2/tmp/p%d_p%d", p_id,0);
    sprintf(ppl_2, "/home/bitnami/650/hw2/hw2/tmp/p%d_p%d", 0,p_id);
    sprintf(ppr_2, "/home/bitnami/650/hw2/hw2/tmp/p%d_p%d", p_id-1,p_id);
    sprintf(ppl_1, "/home/bitnami/650/hw2/hw2/tmp/p%d_p%d", p_id, p_id-1);
  }
  else if(p_id == 0){
    sprintf(ppr_1, "/home/bitnami/650/hw2/hw2/tmp/p%d_p%d", p_id,p_id+1);
    sprintf(ppr_2, "/home/bitnami/650/hw2/hw2/tmp/p%d_p%d", p_num - 1,p_id);
    sprintf(ppl_1, "/home/bitnami/650/hw2/hw2/tmp/p%d_p%d", p_id,p_num-1);
    sprintf(ppl_2, "/home/bitnami/650/hw2/hw2/tmp/p%d_p%d", p_id+1,p_id);
  }
  else{
    sprintf(ppr_1, "/home/bitnami/650/hw2/hw2/tmp/p%d_p%d", p_id,p_id+1);
    sprintf(ppr_2, "/home/bitnami/650/hw2/hw2/tmp/p%d_p%d", p_id-1,p_id);
    sprintf(ppl_2, "/home/bitnami/650/hw2/hw2/tmp/p%d_p%d", p_id+1,p_id);
    sprintf(ppl_1, "/home/bitnami/650/hw2/hw2/tmp/p%d_p%d", p_id,p_id-1);
  }
  if(p_id == p_num - 1){

    ppr_arr[1] = open(ppr_2, O_RDONLY);
    ppr_arr[0] = open(ppr_1, O_WRONLY);
    ppl_arr[1] = open(ppl_2, O_RDONLY);
    ppl_arr[0] = open(ppl_1, O_WRONLY);
  }
  else{
    ppr_arr[0] = open(ppr_1, O_WRONLY);
    ppr_arr[1] = open(ppr_2, O_RDONLY);
    ppl_arr[0] = open(ppl_1, O_WRONLY);
    ppl_arr[1] = open(ppl_2, O_RDONLY);
  }
  
  //srand((unsigned int)time(NULL));
  srand( (unsigned int) time(NULL) + p_id);
  fd_set fds;
  int res;
  char buf[BUFF_LEN];
  
  int  maxfd = max(pm_arr[1], ppr_arr[1],  ppl_arr[1]);
  
  while (1){
	  FD_ZERO(&fds);
	  FD_SET(pm_arr[1], &fds);
	  FD_SET(ppr_arr[1], &fds);
	  FD_SET(ppl_arr[1], &fds); 
	  select(1024, &fds, NULL, NULL, NULL);

    //read from master
    s_potato->msg_type = 'E';
    for (int i = 0;i < 512;i++) {
	s_potato->hop_trace[i] = 0;
    }
    if(FD_ISSET(pm_arr[1], &fds)){
      if(read(pm_arr[1], s_potato, sizeof(*s_potato)) > 0){
	if (s_potato->msg_type == 'E') {
		write(pm_arr[0], s_potato, sizeof(POTATO_T));
		break;
	}
	else if((s_potato->msg_type == 'P') && (s_potato->hop_count > 0)){
	  s_potato->hop_count--;
	  s_potato->hop_trace[s_potato->total_hops-s_potato->hop_count-1] = p_id;
	  // printf("trace here is:%lu\n", s_potato->hop_trace[s_potato->total_hops-s_potato->hop_count-1]);
	  if(s_potato->hop_count <= 0){
	    s_potato->msg_type = 'E';
	    printf("I'm it!\n");
	    write(pm_arr[0], s_potato, sizeof(POTATO_T));
	    //printf("trace after it here is:%lu\n", s_potato->hop_trace[s_potato->total_hops-s_potato->hop_count-1]);
	  }else{
	    //	    printf("send potato to left\n");
	    int nei = rand()%2;
	    int next;
	    if(nei > 0){ //right neighbor
	      if(p_id == p_num-1){
		next = 0;
	      }else{
		next = p_id +1;
	      }
	      write(ppr_arr[0], s_potato, sizeof(POTATO_T));
	      fprintf(stdout, "Sending potato to %d\n",next);
	    }else{ //left
	      if(p_id == 0){
		next = p_num-1;
	      }else{
		next = p_id - 1;
	      }
	      fprintf(stdout, "Sending potato to %d\n",next);
	      write(ppl_arr[0], s_potato, sizeof(POTATO_T));
	    }
	    //write(ppl_arr[0], s_potato, sizeof(POTATO_T));
	  }
	}//if able to keep passing
      }//if read master > 0
    }//select master
    else if(FD_ISSET(ppr_arr[1], &fds)) {
	read(ppr_arr[1], s_potato, sizeof(*s_potato));
	if((s_potato->msg_type != 'E') && (s_potato->hop_count > 0)){
		s_potato->hop_count--;
		s_potato->hop_trace[s_potato->total_hops-s_potato->hop_count-1] = p_id;
		//	printf("trace here is:%lu\n", s_potato->hop_trace[s_potato->total_hops-s_potato->hop_count-1]);
		if(s_potato->hop_count <= 0){
			printf("I'm it!\n");
			s_potato->msg_type = 'E';
			write(pm_arr[0], s_potato, sizeof(POTATO_T));
			//	printf("trace after it here is:%lu\n", s_potato->hop_trace[s_potato->total_hops-s_potato->hop_count-1]);
		}else{
		  int nei = rand()%2;
		  int next;
		  if(nei > 0){ //right neighbor
		    if(p_id == p_num-1){
		      next = 0;
		    }else{
		      next = p_id +1;
		    }
		    write(ppr_arr[0], s_potato, sizeof(POTATO_T));
		    fprintf(stdout, "Sending potato to %d\n",next);
		  }else{ //left
		    if(p_id == 0){
		      next = p_num-1;
		    }else{
		      next = p_id - 1;
		    }
		    fprintf(stdout, "Sending potato to %d\n",next);
		    write(ppl_arr[0], s_potato, sizeof(POTATO_T));
		  }
		  //	  printf("send potato to left\n");
		  //write(ppl_arr[0], s_potato, sizeof(POTATO_T));
		}
	}
	else {
		write(pm_arr[0], s_potato, sizeof(POTATO_T));
		break;
	} 
    }
    else if (FD_ISSET(ppl_arr[1], &fds)) {
	read(ppl_arr[1], s_potato, sizeof(*s_potato));
	if((s_potato->msg_type != 'E') && (s_potato->hop_count > 0)){
		s_potato->hop_count--;
		s_potato->hop_trace[s_potato->total_hops-s_potato->hop_count-1] = p_id;
		//printf("trace here is:%lu\n", s_potato->hop_trace[s_potato->total_hops-s_potato->hop_count-1]);
		if(s_potato->hop_count <= 0){
			printf("I'm it!\n");
			s_potato->msg_type = 'E';
			write(pm_arr[0], s_potato, sizeof(POTATO_T));
			//	printf("trace after it here is:%lu\n", s_potato->hop_trace[s_potato->total_hops-s_potato->hop_count-1]);
		}else{
		  int nei = rand()%2;
		  int next;
		  if(nei > 0){ //right neighbor
		    if(p_id == p_num-1){
		      next = 0;
		    }else{
		      next = p_id +1;
		    }
		    write(ppr_arr[0], s_potato, sizeof(POTATO_T));
		    fprintf(stdout, "Sending potato to %d\n",next);
		  }else{ //left
		    if(p_id == 0){
		      next = p_num-1;
		    }else{
		      next = p_id - 1;
		    }
		    fprintf(stdout, "Sending potato to %d\n",next);
		    write(ppl_arr[0], s_potato, sizeof(POTATO_T));
		  }
		  //  printf("send potato to left\n");
		  //	write(ppl_arr[0], s_potato, sizeof(POTATO_T));
	  }
	}
	else {
		write(pm_arr[0], s_potato, sizeof(POTATO_T));
		break;
	}
    }
  }//while(1)
  sleep(3);
  //printf("DEBUG: player close:\n");
  for(int i = 0; i < 2; i++){
    close(pm_arr[i]);
    close(ppr_arr[i]);
    close(ppl_arr[i]);
  }
 
  //  free(s_potato);
  return p_state;
}

int main(int argc, char *argv[]){
  int count = 0;
  if(argc != 2){
    fprintf(stderr, "number of arguments is not\
 right!\nUseage: player <player_id>\n");
    return EXIT_FAILURE;
  }
  int arg_id;
  //  printf("DEBUG: argv0 is:%s\n", argv[0]);
  char* ptr1 = strchr(argv[0],'\0');
  argv[1] = skipWs(ptr1+1);
  char* arg1 = argv[1];
  // printf("DEBUG: argv1 is:%s, int is:%d\n", argv[1], atoi(argv[1]));
  if(!readInt(&arg1, &arg_id)){
    fprintf(stderr, "Second argument should be an integer\n");
    // printf("DEBUG: err arg_id is: %s, int is: %d\n", arg1, atoi(arg1));
    return EXIT_FAILURE;
  }
  else{
    //   printf("DEBUG:Now the input should be valid\n");
    /*player execution process*/
    if(!playerExec(atoi(argv[1]))){
      // fprintf(stderr,"Something bad happend when executing player program\n");
      return EXIT_FAILURE;
    }
  }
  //exit(0);
  //
  return EXIT_SUCCESS;
}
