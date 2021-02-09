#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netdb.h>

// TODO: 
int writen(int new_fd, char* buffer){
    int err = send(new_fd, buffer, strlen(buffer), 0);
    if (err < 0){
        perror("send() error");
        exit(EXIT_FAILURE);
    }
    printf("[+] MSG SENT: %s\n", buffer);

	return err;
}

// TODO:
int readline(int new_fd, char* buffer) {
    int err = recv(new_fd, buffer, sizeof(buffer), 0);
    if (err < 0){
        perror("recv() error");
        exit(EXIT_FAILURE);
    }
    printf("[+] MSG RECIEVED: %s\n", buffer);
    
    return err;
}