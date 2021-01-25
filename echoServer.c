#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netdb.h>
//Including libraries required for scoket proramming

#define PORT 8080
#define BUFFER 1024

int main(){

  //create variables
  int sfd, new_fd;
	int opt = 1;
	struct sockaddr_in addr_serv, addr_client;
	struct hostent addr_host;
	char buffer[BUFFER];
	char ip_addr[] = "127.0.0.1";

	printf("[*] INITIATE SERVER AT %s PORT %d\n",ip_addr,PORT);

	if((sfd = socket(AF_INET,SOCK_STREAM,0)) == 0){
		perror("socket");
		exit(EXIT_FAILURE);
	};

	printf("[*] SOCKET SUCCESSFUL \n");

	if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt))) { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
	}
	printf("[*] SETSOCKOPT SUCCESSFUL \n");
	
	memset(&addr_serv,0,sizeof(addr_serv));
	addr_serv.sin_family = AF_INET;
	addr_serv.sin_addr.s_addr =  inet_addr(ip_addr); //htonl(INADDR_ANY);s
	addr_serv.sin_port = htons(PORT);

	if(bind(sfd,(struct sockaddr *)&addr_serv,sizeof(addr_serv))!= 0){
		perror("bind");
		exit(EXIT_FAILURE);
	}
	printf("[*] BIND SUCCESSFUL \n");
	
	if(listen(sfd,5) != 0){
		perror("listen");
		exit(EXIT_FAILURE);
	}
	printf("[*] LISTENING SUCCESSFUL \n");
	
	int client_addr_len = sizeof(addr_client);
	while(1){
		
		if((new_fd = accept(sfd,(struct sockaddr*)&addr_client, &client_addr_len)) < 0){
			perror("accept");
			exit(EXIT_FAILURE);
		}
		printf("[*] CONNECTION ACCEPTED \n");
		
		if(recv(new_fd,buffer,sizeof(buffer),0) <0){
			perror("recv");
			exit(EXIT_FAILURE);
		}
		printf("Received String : %s\n",buffer);

		if(send(new_fd,buffer,strlen(buffer),0) <0){
			perror("send");
			exit(EXIT_FAILURE);
		}
		printf("Sent string : %s\n",buffer);

		shutdown(new_fd,SHUT_RDWR);

	}
	close(new_fd);
return 0;

}