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
 * 
 * EINTR is an error 
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

int readline2(int socketfd, char* buffer, int numBytes) {
    int err_code = 0;

    for (int byte = 0; byte < numBytes; byte += err_code) {
        err_code = recv(socketfd, buffer, sizeof(buffer), 0);

        // If EOF, then exit the loop
        if (err_code == 0 || strstr(buffer, '\n') != NULL) {
            break;
        }

        // Retry reading if EINTR and the error code == -1
        if (errno == EINTR && err_code == -1) {
            continue;
        } else if (err_code == -1) {
            perror("readline() error");
            return -1;
        }
    }

    return err_code;
}