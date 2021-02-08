#include "common.h"

/**
 * Client uses IPV4 structure for sockets.
 * 
 * Start the client with command "echo <IPAdr> <Port>".
 * 
 * Client will then read a line of text from standard input and write
 * the line to the network socket connected to the server. 
 * 
 * Server will echo back the message, and the client will print
 * the message out.
 * 
 * Client will close its socket and exit when it reads an EOF from 
 * standard input (Ctrl-D). When it closes the socket, it will send a
 * TCP FIN packet to the server, and the server child process will return
 * with a 0. 
 * 
 *  TODO: need to keep client connected and read in from stdin
 *  TODO: exit from loop once the input is CTRL-D
 **/

#define BUFFER 1024

int main(int argc, char *argv[]){
    struct sockaddr_in addr_server;
    int cfd;
    char  buffer[BUFFER];

    printf("args: %s %s\n", argv[0], argv[1]);

    if (argc != 3){
        perror("Usage: echo <IPAddress> <Port>");
        exit(EXIT_FAILURE);
    }
    printf("[+] ECHO %s \n", argv[1]);

    char ip_addr[] = "127.0.0.1"; // argv[1];
    int port = 8080; // argv[2];

    // Initialize socket using TCP. TODO: does this need to be < 0?
    if ((cfd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("client socket() error");
        exit(EXIT_FAILURE);
	};
    printf("[+] SOCKET CREATED \n");

    addr_server.sin_family = AF_INET;
    //addr_server.sin_addr.s_addr = inet_addr(ip_addr);
    addr_server.sin_port = htons(port);
  
    // Convert from ASCII string to binary network address
    if (inet_pton(AF_INET, ip_addr, &addr_server.sin_addr) <= 0){ 
        perror("client inet_pton() error"); 
        exit(EXIT_FAILURE);
    } 
  
    // Connect to the server. NOTE changed cast from sockadd_in to sockaddr
    if (connect(cfd, (struct sockaddr *)&addr_server, sizeof(addr_server)) < 0){
        perror("client connect() error");
        exit(EXIT_FAILURE);
    } 
    printf("[+] CONNECTED TO SERVER \n");

    char message[100];
    while (1){
        printf("Enter message: ");
        scanf("%s", message);

        // if (message == EOF) { break; }

        // Send message to the server
        if (send(cfd, message, sizeof(message), 0) < 0){
            perror("client send() error");
            exit(EXIT_FAILURE);
        }
        printf("[+] MSG SENT \n");

        // Recieve message back from server
        if (recv(cfd, buffer, sizeof(buffer), 0) < 0){
            perror("client recv() error");
            exit(EXIT_FAILURE);
        }
        printf("[+] ECHOED MSG : %s \n", buffer);
    }

    // need to close socket and send TCP FIN packet to server here
    shutdown(cfd, SHUT_RDWR);
    printf("[+] CONNECTION TO SERVER CLOSED \n");
    return 0;
}