#include "common.h"

/**
 * Server uses IPV4 structure for sockets.
 * 
 * Start the server with the command "echos <Port>".
 * 
 * Server will then wait for a client to connect and will read in messages. 
 * It will then echo back all messages the client sends. 
 * 
 * Once a client closes, it will send the server a TCP FIN packet. The server
 * child process read() command will return a 0, and then the child process will
 * be closed.
 * 
 * TODO: does there need to be new threads to manage each new client
 **/

#define BUFFER 1024
#define IP_ADDRESS "127.0.0.1"
#define BACKLOG 10

int main(int argc, char *argv[]){
  	int sfd, new_fd;
	int opt = 1;
	struct sockaddr_in addr_serv, addr_client;
	struct hostent addr_host;
	char buffer[BUFFER];
	pid_t child_pid;
	int client_addr_len;

    if (argc != 2){
        perror("Usage: ./echos <Port>");
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
	printf("[*] INITIATE SERVER AT %s PORT %d\n", IP_ADDRESS, port);

	// Initialize the socket
	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
		perror("server socket() error");
		exit(EXIT_FAILURE);
	};
	printf("[*] SOCKET SUCCESSFUL \n");

	if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { 
        perror("server setsockopt() error"); 
        exit(EXIT_FAILURE); 
	}
	printf("[*] SETSOCKOPT SUCCESSFUL \n");
	
	memset(&addr_serv, 0, sizeof(addr_serv));
	addr_serv.sin_family = AF_INET;
	addr_serv.sin_addr.s_addr =  inet_addr(IP_ADDRESS); // htonl(INADDR_ANY);
	addr_serv.sin_port = htons(port);

	// Assign local protocol address to socket
	if (bind(sfd, (struct sockaddr *)&addr_serv, sizeof(addr_serv)) != 0){
		perror("server bind() error");
		exit(EXIT_FAILURE);
	}
	printf("[*] BIND SUCCESSFUL \n");
	
	// Convert unconnected socket to a passive socket with max number of clients
	if (listen(sfd, BACKLOG) != 0){
		perror("server listen() error");
		exit(EXIT_FAILURE);
	}
	printf("[*] LISTENING SUCCESSFUL \n");
	
	// Accepts up to 10 clients 
	while(1){

		// Returns the next completed connection from completed connection queue
		client_addr_len = sizeof(addr_client);
		if ((new_fd = accept(sfd, (struct sockaddr *)&addr_client, &client_addr_len)) < 0){
			perror("server: accept() error");
			exit(EXIT_FAILURE);
		}
		printf("[*] CONNECTION ACCEPTED \n");

		// Create new child process for the incoming client
		if ((child_pid = fork()) < 0) {
			perror("server: child fork() error");
			return 0;
		}

		if (child_pid == 0) {
            // Read message from client
            readline(new_fd, buffer);

            // Send back echo
            writen(new_fd, buffer);
            
		    exit(EXIT_SUCCESS);
		}
	}

	shutdown(new_fd, SHUT_RDWR);
	close(new_fd);
	return 0;
}