#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define BUFFER 1024
#define PORT 8080

int main(int argc,char *argv[]){

  struct sockaddr_in addr_server;
  int cfd, msg_len;
  char ip[] = "127.0.0.1";
  char  buffer[BUFFER];

  if(argc != 2){
    perror("No Arguments");
    exit(EXIT_FAILURE);
  }
  printf("[+] ECHO %s \n",argv[1]);
  msg_len = sizeof(argv[1]);

  if((cfd = socket(AF_INET,SOCK_STREAM,0)) == 0){
		perror("socket");
		exit(EXIT_FAILURE);
	};


  printf("[+] SOCKET CREATED \n");

  addr_server.sin_family = AF_INET;
  //addr_server.sin_addr.s_addr = inet_addr(ip);
  addr_server.sin_port = htons(PORT);
  
  if(inet_pton(AF_INET,ip, &addr_server.sin_addr) <= 0){ 
        perror("inet_pton"); 
        exit(EXIT_FAILURE);
      
  } 
  
  if(connect(cfd, (struct sockaddr_in *)&addr_server,sizeof(addr_server))< 0){
    perror("connect");
    exit(EXIT_FAILURE);
  } 
  printf("[+] CONNECTED \n");
  while (1){
    if(send(cfd,argv[1],msg_len,0) < 0 ){
      perror("send");
      exit(EXIT_FAILURE);
    }
    printf("[+] MSG SENT \n");

    if(recv(cfd,buffer,sizeof(buffer),0) < 0){
      perror("recv");
      exit(EXIT_FAILURE);
    }
    printf("[+] ECHOED MSG : %s \n",buffer);

    shutdown(cfd,SHUT_RDWR);
  }

  return 0;

}