#include"common.h"

#define WAIT 15

void join_server(int fd, char* user_name,int len){
  struct sbcp_message join;
  memset(&join,0,sizeof(join));

  join.vrsn = 3;
  join.type = JOIN;
  join.length = 6; //size of packet == 6 Bytes
  join.msg_payload.type = USERNAME;
  join.msg_payload.length = (4+len);
  for(int i=0;i < len;i++){
    join.msg_payload.payload[i]=user_name[i];
  } 
  pack(join);
  if(send(fd, msg_, sizeof(msg_), 0) < 0){
    perror("join error : send()");
    exit(EXIT_FAILURE);
  }
}

void read_(char msg[]){

  struct sbcp_message read_msg;
  memset(&read_msg,0,sizeof(read_msg));

  read_msg = unpack(msg);
  printf("%s\n",read_msg.msg_payload.payload);

  switch (read_msg.type)
  {
  case ACK:
    printf("ACK : %s\n",read_msg.msg_payload.payload);
    break;
  case NAK:
    printf("NAK : %s",read_msg.msg_payload.payload); 
  case ONLINE:
    printf("ONLINE : %s\n",read_msg.msg_payload.payload);
    break;
  case OFFLINE:
    printf("OFFLINE : %s\n",read_msg.msg_payload.payload); 
    break;
  case FWD:
    printf("NEW MESSAGE : %s\n",read_msg.msg_payload.payload);
    break;
  default:
    printf("GOT : %s\n",read_msg.msg_payload.payload);
    break;
  } 
}

int main(int argc, char *argv[]){

    struct sockaddr_in addr_server;
    struct timeval timeout;
    int cfd;
    fd_set read_fd;
    char username[16];
    char attr_payload[512];
    char recv_message[2000];
    int payload_size = sizeof(attr_payload);
    int startup = 0;

    // if (argc != 3){
    //     perror("Usage: sbcp_c <IPAddress> <Port>\n");
    //     exit(EXIT_FAILURE);
    // }

    memset(username, '\0', sizeof(username));
    printf("Enter username :\n");
    fgets(username,16,stdin); //fgets to limit id to 16

    char ip_addr[10] = "127.0.0.1";//argv[1];
    int port = 9034;//atoi(argv[2]);
    
    /* ---- Set up client ------ */
    // Initialize socket using TCP
    if ((cfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){ //IP agnostic
        perror("client socket() error");
        exit(EXIT_FAILURE);
	};
    printf("[+] SOCKET CREATED \n");

    addr_server.sin_family = AF_UNSPEC;
    addr_server.sin_port = htons(port);
  
    // Convert from ASCII string to binary network address
    //AF_INET here because tamu ece servers block IPv6
    if (inet_pton(AF_INET, "127.0.0.1", &addr_server.sin_addr) <= 0){  
        perror("client inet_pton() error"); 
        exit(EXIT_FAILURE);
    } 
  
    // Connect to the server
    if (connect(cfd, (struct sockaddr *)&addr_server, sizeof(addr_server)) < 0){
        perror("client connect() error");
        exit(EXIT_FAILURE);
    } 
    printf("[+] CONNECTED TO SERVER \n");

    //join_server(cfd,username,sizeof(username));
    

    timeout.tv_sec = WAIT;
    timeout.tv_usec = 0;

    /*
    iterarte through all fd in fd_set and check for any event in an infinite loop.
    0 --> send to server
    cfd --> revcieve from server  
    and ignores anyother fd because it is a client and doesnt have to connect to anyone else
    */
    while(1){
      FD_ZERO(&read_fd);
      FD_SET(0,&read_fd);
      FD_SET(cfd,&read_fd);
      if(select(cfd+1,&read_fd,NULL,NULL,&timeout)<0){
        perror("client select() error");
        exit(EXIT_FAILURE);
      }
      else{
        if(startup==0){
          send_message(cfd,JOIN,USERNAME,username,sizeof(username));
          startup++;
        }
        
        if(FD_ISSET(cfd,&read_fd)){
          memset(&recv_message,0,sizeof(recv_message));
          if (recv(cfd,recv_message,sizeof(recv_message),0) < 0){
            perror("client recv() error");
          }
        } 

        if(FD_ISSET(0,&read_fd)){
          memset(&attr_payload,0,payload_size);
          fgets(attr_payload,payload_size-1,stdin);
          send_message(cfd,SEND,MSG,attr_payload,sizeof(attr_payload));
        }

      }
    }
    
    return 0;
}