#include "common.h"

#define MAX_BYTES 1024
#define BACKLOG 10

#define IP_ADDRESS "127.0.0.1"
#define PORT 9304
#define MAX_CLIENTS 10
#define GREETING "Connected to SBCP Server v1.0 \n"

// get sockaddr, IPv4 or IPv6
void* get_in_addr(struct sockaddr* sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/**
 * If the FD is within the FD set, then handle the new connection and 
 * add the new fd to the master fd set.
 * 
 * NOTE: master_fds needs to be a pointer so that it can update the original 
 * FD set in main()
 * 
 * NOTE: need to check usernames for each connection
 */
int accept_new_connection(int fd, fd_set* master_fds, int fdmax, int listener) {
    struct sockaddr_storage remoteaddr;
    int newfd;
    socklen_t addrlen;

    char remoteIP[INET6_ADDRSTRLEN];
    // If there are new connections, handle them
    if (fd == listener) {
        addrlen = sizeof(remoteaddr);
        if ((newfd = accept(listener, (struct sockaddr*)&remoteaddr, &addrlen)) < 0) {
            perror("server accept() error");
            return -1;
        }
    }

    else {
        FD_SET(newfd, master_fds);
        if (newfd > fdmax) {
            fdmax = newfd;
        }
        printf("select server: new connection from %s on socket %d \n", 
            inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*) &remoteaddr),
            remoteIP, INET6_ADDRSTRLEN), newfd);
    }
}


/**
 * Broadcast the message to the other clients
 */
void broadcast_message(int client_fd, fd_set* master_fds, int fdmax, int listener) {
    int nbytes;
    char buf[MAX_BYTES];
    // struct sbcp_message read_msg;
    // memset(&read_msg,0, sizeof(read_msg));
    // read_msg = unpack(buf);
    // printf("%s \n", read_msg.msg_payload.payload);

    if ((nbytes = recv(client_fd, buf, sizeof(buf), 0)) <= 0) {
        // Got error or connection closed by client
        if (nbytes == 0) {
            printf("select server: socket %d hung up \n", client_fd);
        } else {
            perror("server recv error");
        }
        close(client_fd);
        FD_CLR(client_fd, master_fds); // remove fd from master set

    } else {
        for (int fd = 0; fd <= fdmax; ++fd) {
            // send message to everyone except the listeners and the senderS
            if (FD_ISSET(fd, master_fds)) {
                if (fd != listener && fd != client_fd) {
                    if (send(fd, buf, nbytes, 0) == -1) {
                        perror("server send error");
                    }
                }
            }
        }
    }
}


int main(int argc, char* argv[]) {
    fd_set master_fds;      // master file descriptor list
    fd_set read_fds;        // temp file descriptor list for select()
    int fdmax;              // max file descriptor number

    int opt = 1;
    int listener;
    struct sockaddr_in address;

    int nbytes;

    if (argc != 4) {
        perror("Usage: <./server server_ip server_port max_clients>");
        exit(EXIT_FAILURE);
    }

    char ip_address = IP_ADDRESS; // argv[1];
    int port = PORT; // atoi(argv[2]);
    int max_clients = MAX_CLIENTS; // atoi(argv[3]);

    FD_ZERO(&master_fds);
    FD_ZERO(&read_fds);

    if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    printf("[*] SOCKET SUCCESSFUL \n");

    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("server setsockopt() error");
        exit(EXIT_FAILURE);
    }
    printf("[*] SETSOCKOPT SUCCESSFUL \n");

    memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(IP_ADDRESS);
	address.sin_port = htons(port);

    if (bind(listener, (struct sockaddr *)&address, sizeof(address)) < 0) 
    {
        perror("server bind() failed");
        exit(EXIT_FAILURE);
    }
	printf("[*] BIND SUCCESSFUL \n");

    if (listen(listener, BACKLOG) < 0) {
        perror("server listen() failed");
        exit(EXIT_FAILURE);
    }
    printf("[*] LISTENING SUCCESSFUL \n");

    // Add listener to master set and keep track of biggest file descriptor
    FD_SET(listener, &master_fds);
    fdmax = listener;
    printf("%d",listener);
    printf("WAITING FOR CLIENTS TO CONNECT... \n");
    while (1) {
        read_fds = master_fds;
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("server select() error");
            exit(EXIT_FAILURE);
        }

        // run through the existing connections looking for data to read
        for (int fd = 0; fd <= fdmax; ++fd) {
            if (FD_ISSET(fd, &read_fds)) {
                if (fd == listener) {
                    accept_new_connection(fd, &master_fds, fdmax, listener);
                }
            } else {
                broadcast_message(fd, &master_fds, fdmax, listener);
            }
        }
    }

    return 0;
}