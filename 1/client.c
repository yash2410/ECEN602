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
#define MAX_BYTES 256

int main(int argc, char *argv[]){
    struct sockaddr_in addr_server;
    int cfd;
    char  buffer[BUFFER];

    if (argc != 3){
        perror("Usage: echo <IPAddress> <Port>");
        exit(EXIT_FAILURE);
    }
    printf("[+] ECHO %s \n", argv[1]);

    char* ip_addr = argv[1];
    int port = atoi(argv[2]);

    /* ---- Set up client ------ */
    // Initialize socket using TCP
    if ((cfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
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
  
    // Connect to the server
    if (connect(cfd, (struct sockaddr *)&addr_server, sizeof(addr_server)) < 0){
        perror("client connect() error");
        exit(EXIT_FAILURE);
    } 
    printf("[+] CONNECTED TO SERVER \n");

    /* ----- Communicate with server ---- */
    char message[100];
    printf("Enter message: ");
    // scanf("%s", message);
    fgets(message, MAX_BYTES, stdin);

    // Send message to server
    writen(cfd, message);

    // Echo back response
    readline(cfd, buffer, MAX_BYTES);

    // Close socket and send TCP FIN packet to server
    shutdown(cfd, SHUT_RDWR);
    printf("[+] CONNECTION TO SERVER CLOSED \n");

    return 0;
}