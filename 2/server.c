#include "common.h"

/**
 * server.c
 * 
 * One instance of the SBCP server will provide a single "chat room" which can
 * handle a finite amount of clients. 
 * 
 * -------------- Operations ---------------
 * 1. The server should start on the IP and port supplied on the command line
 *      <./server server_ip server_port max_clients>
 * 2. A SEND received by the server will cause a copy of the MESSAGE text to be sent
 *      in FWD to all clients except the original sender
 * 3. A server may only accept JOIN SBCP messages from unknown clients. The server will
 *      not allow multiple clients to use the same username.
 * 4. Clients may leave the chat session unceremoniously. The server should cleanup its
 *      resources (close socket, make the username available)
 * 5. The server should discard any messages that are not understood.
 * 6. The implementation can be an iterative server. A distinct TCP connection is used for
 *      each client-server transaction.
 * 
 * -------------- I/O Multiplexing ------------
 * Need to use a single server process that can deal with a listening socket for new 
 * clients and additional connected sockets (one socket for each client that is connected).
 * 
 * The TCP server will use I/O multiplexing to handle both a listening socket as well as 
 * its connected sockets. 
 * 
 * select() blocks the program until input or output is ready on a specified set of 
 * file descriptors, or until a timer expires, whichever comes first.
 * - configure the sets for reads, writes, and exceptions
 * - to test for which events are set when select returns
 * 
 * How to use select in the chat server:
 * 1. First call socket, bind, and listen to setup listening socket
 * 2. Next you set up the data structure for select and call select to wait for an event.
 * 3. Calling select causes your program to block until one of the events you're waiting for
 *      is set.
 * 4. When select returns, you need to check what event took place and then process the event.
 *      - e.g. issue a read for the socket that has data
 * 5. If there is new activity on the listening socket, you need to call accept to connect
 *      to the new client. 
 * 6. A new client will require that you add this new connected socket to your list of connected
 *      sockets and update the select data structure before calling the next select.
 * 
 * Can get away with just issuing select for descriptors that are ready for reading and accepting
 * a connection and not worry about writing.
 * 
 */

#define BUFFER 1024
#define IP_ADDRESS "127.0.0.1"
#define BACKLOG 10
#define MAX_BYTES 256

int numClients = 0;

/**
 * Checks if username exists 
 */
int doesUserNameExist(char a[], int maxNumClients) {
    return 0;
}

/**
 * Broadcast message to all other clients.
 * The broadcast is done by keeping a list of all the connected clients and their
 * file descriptors updated, and by iterating through it to send each client the message.
 */ 
void broadcastMessage(struct sbcp_message msg, fd_set clientFdList, int listenFd, int acceptFd, int latestFd) {
    for (int i = 0; i <= latestFd; ++i) {

        // check if the broadcasting fd is in the client fd_set
        if (FD_ISSET(i, &clientFdList)) {

            // except for the listener socket and ourselves
            if (listenFd != i && acceptFd != i) {
                if ((write(i, (void*)&msg, sizeof(msg))) == -1) {
                    printf("Server cannot broadcast. \n");
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
  	int master_fd, new_fd;
	int opt = 1;
	struct sockaddr_in addr_serv, addr_client;
	struct hostent addr_host;
	char buffer[BUFFER];
	pid_t child_pid;
	int client_addr_len;

    int client_socket[30];

    // TODO: why is there 2 fd_sets in this example??
    fd_set master;      // master file descriptor list
    fd_set read_fds;    // temp file descriptor list of select()
    int fdmax;          // maximum file descriptor number
    int timeout = 10;   // max timeout TODO: write type and length??


    if (argc != 3) {
        perror("Usage: <./server server_ip server_port max_clients>");
        exit(EXIT_FAILURE);
    }

    // Initialize all client_socket[] to 0 so not checked (argv[2] == max_clients)
    for (int i = 0; i < argv[2]; ++i) {
        client_socket[i] = 0;
    }

    // Clear the master and temp fd sets
    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    // --------- Create, bind, and listen listening socket -----------
    int port = atoi(argv[1]);
	printf("[*] INITIATE SERVER AT %s PORT %d\n", IP_ADDRESS, port);

	if ((master_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
		perror("server socket() error");
		exit(EXIT_FAILURE);
	};
	printf("[*] SOCKET() SUCCESSFUL \n");

	if (setsockopt(master_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { 
        perror("server setsockopt() error"); 
        exit(EXIT_FAILURE); 
	}
	printf("[*] SETSOCKOPT() SUCCESSFUL \n");
	
	memset(&addr_serv, 0, sizeof(addr_serv));
	addr_serv.sin_family = AF_INET;
	addr_serv.sin_addr.s_addr =  inet_addr(IP_ADDRESS);
	addr_serv.sin_port = htons(port);

	if (bind(master_fd, (struct sockaddr*) &addr_serv, sizeof(addr_serv)) < 0){
		perror("server bind() error");
		exit(EXIT_FAILURE);
	}
	printf("[*] BIND SUCCESSFUL() \n");
	
	if (listen(master_fd, BACKLOG) < 0){
		perror("server listen() error");
		exit(EXIT_FAILURE);
	}
	printf("[*] LISTENING SUCCESSFUL() \n");

    // -------------------------------------------------

    FD_SET(listen_fd, &master);
    fdmax = listen_fd;  // setting fdmax to current maximum fd value

    while (1) {
        // copy master to read_fds, later master would be updated
        read_fds = master;

        if (select(fdmax + 1, &read_fds, NULL, NULL, &timeout) == -1) {
            perror("server select error");
            exit(EXIT_FAILURE);
        }

        // run through existing connections looking for data to read
        for (int i = 0; i < fdmax; ++i) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == listen_fd){
                    // accept a new client connection
                } else {
                    // you have messages (JOIN/SEND/IDLE) from other clients
                }
            }
        }

    }
}