#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

/**
 * Write n bytes to a socket and return the number of bytes writen or a -1 on error.
 * Might input or output fewer bytes than expected due to buffer limits, but this can
 * be resolved by calling write again. Only seen on write if the socket is non-blocking.
 **/
int writen(int socketfd, char* buffer){
    int err = send(socketfd, buffer, strlen(buffer), 0);
    if (err < 0){
        perror("send() error");
        exit(EXIT_FAILURE);
    }
    printf("[+] MSG SENT: %s\n", buffer);

	return err;
}

/**
 * Will read a line of characters from a socket and return the number of bytes
 * read or a -1 on error.  
 **/
int readline(int socketfd, char* buffer, int numBytes) {
    int err = recv(socketfd, buffer, sizeof(buffer), 0);
    if (err < 0){
        perror("recv() error");
        exit(EXIT_FAILURE);
    }
    printf("[+] MSG RECIEVED: %s\n", buffer);
    
    return err;
}

#endif