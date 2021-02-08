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
	return 0;
}

// TODO:
int readline(int new_fd, char* buffer) {
    if (recv(new_fd, buffer, sizeof(buffer), 0) < 0){
        perror("server recv() error");
        exit(EXIT_FAILURE);
    }
    printf("Received string : %s\n", buffer);
    
}