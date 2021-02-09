#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netdb.h>


int writen(int socketfd, char* buffer){
    int err = send(socketfd, buffer, strlen(buffer), 0);
    if (err < 0){
        perror("send() error");
        exit(EXIT_FAILURE);
    }
    printf("[+] MSG SENT: %s\n", buffer);

	return err;
}

int readline(int socketfd, char* buffer) {
    int err = recv(socketfd, buffer, sizeof(buffer), 0);
    if (err < 0){
        perror("recv() error");
        exit(EXIT_FAILURE);
    }
    printf("[+] MSG RECIEVED: %s\n", buffer);
    
    return err;
}