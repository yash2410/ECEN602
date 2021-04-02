#include "constants.c"
/**
 * Includes functions for sending and receiving requests.
 * client request : takes client_info generates a socket specific to the client and sorts into rrq or wrq
 * rrq : gets the filename and data to send to the client as well as handles ACKs
 * error : sends error to the clients
 * data : sends data packets on request from client
 */
void data(struct data data,struct client_info client){
  data.opcode = htons(DATA);
  int i = sendto(client.sock,&data,sizeof(&data),0, (struct sockaddr *)&client.client, sizeof(client.client));
    if(i<0){
      perror("error()");
    }
}
/**
 * Error
 */
void error(struct client_info client,char error_string){
    struct error error;
    error.opcode = htons(ERROR);
    error.error_num = 0;
    int i = sendto(client.sock,&error,sizeof(&error),0, (struct sockaddr *)&client.client, sizeof(client.client));
    if(i<0){
      perror("error()");
    }
}

/**
 * Read Request Function
 */
void rrq(struct client_info client)
{
  char *filename;
  char *mode_s;
  int block_num = 0;
  filename = client.buffer.file;
  mode_s = strchr(filename, '\0') + 1; 
  FILE *fd = fopen(fopen,"r");
  int close = 0;
  if(fd == NULL){
    error(client,"file not found");
  }
  struct data data_;
  while(!close){
    int data_len = fread(data_.data,1,sizeof(data_.data),fd);
    block_num++;

    if(data_len < sizeof(data_.data)){
      close = 1;
    }

    data_.block_number = htons(block_num);
    data(data_,client);
    client.size = recvfrom(client.sock, &client.buffer, sizeof(client.buffer), 0, (struct sockaddr *)&client.client, sizeof(client.client));
    if(client.size<0){
      perror("rrq_recicvfrom");
    }
    printf("ACK for block number %u",data_.block_number);
  }

}
/**
 * BONUS: Write Request function
 */
void wrq(struct client_info client)
{

}
/**
 * Client Request
*/
int client_request(struct client_info client){
  int opcode = htons(client.buffer.opcode);
  int sock;
  sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
  if(sock  < 0){
    return -1;
  }
  client.sock = sock;
  if(opcode = RRQ){
    rrq(client);
  }else{
    wrq(client);
  }   
  pclose(sock);
  return 0;
}