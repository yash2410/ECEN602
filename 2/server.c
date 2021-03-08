#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT "9034"
#define MAX_BYTES 1024 // 256?
#define BACKLOG 10

#define IP_ADDRESS "127.0.0.1"

// get sockaddr, IPv4 or IPv6
void* get_in_addr(struct sockaddr* sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Return a listening socket
int get_listener_socket() {
    int listener;   // Listening socket descripter
    int opt = 1;    // For setsockopt() SO_REUSEADDR, below
    int rv;

    struct addrinfo hints, *ai, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "poll server: %s \n", gai_strerror(rv));
        return -1;
    }

    for (p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            continue;
        }

        // Get rid of "address already in use" error message
        if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int))) {
            close(listener);
            continue;
        }


        // Bind listener socket to a port
        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    // Didn't get bound
    if (p == NULL) {
        fprintf(stderr, "select server: failed to bind \n");
        return -1;
    }

    freeaddrinfo(ai);

    if (listen(listener, BACKLOG) == -1) {
        perror("server listen error");
        return -1;
    }

    return listener;
}

/**
 * If the FD is within the FD set, then handle the new connection and 
 * add the new fd to the master fd set.
 * 
 * NOTE: master_fds needs to be a pointer so that it can update the original 
 * FD set in main()
 */
void accept_new_connection(int fd, fd_set* master_fds, int fdmax, int listener) {
    struct sockaddr_storage remoteaddr;
    int newfd;
    socklen_t addrlen;

    char remoteIP[INET6_ADDRSTRLEN];

    // If there are new connections, handle them
    if (fd == listener) {
        addrlen = sizeof(remoteaddr);
        newfd = accept(listener, (struct sockaddr*)&remoteaddr, &addrlen);
        if (newfd == -1) {
            perror("accept");
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
 * If the FD is not within the FD set, then handle the data sent from the 
 * client to the server.
 */
void handle_client_messages(int fd, fd_set* master_fds, int fdmax, char* buf, int listener) {
    int nbytes;

    // Got error or connection closed by client
    if ((nbytes = recv(fd, buf, sizeof(buf), 0)) <= 0) {
        if (nbytes == 0) {
            printf("select server: socket %d hung up \n", fd);
        } else {
            perror("server recv error");
        }
        close(fd);
        FD_CLR(fd, master_fds); // remove from master set
    } 
    
    // We got some data from a client
    else {
        if ((nbytes = recv(fd, buf, sizeof(buf), 0)) <= 0) {
            // Got error or connection closed by client
            if (nbytes == 0) {
                printf("select server: socket %d hung up \n", fd);
            } else {
                perror("server recv error");
            }
            close(fd);
            FD_CLR(fd, master_fds);

        } else {
            for (int i = 0; i <= fdmax; ++i) {
                // send to everyone except the listener and ourselves
                if (FD_ISSET(i, master_fds)) {
                    if (i != listener && i != fd) {
                        if (send(i, buf, nbytes, 0) == -1) {
                            perror("server send error");
                        }
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
    char buf[MAX_BYTES];  // holds data from clients until it can be processed

    if (argc != 3) {
        perror("Usage: <./server server_ip server_port max_clients>");
        exit(EXIT_FAILURE);
    }

    FD_ZERO(&master_fds);
    FD_ZERO(&read_fds);

    int listener = get_listener_socket();
    if (listener == -1) {
        fprintf(stderr, "error creating listening socket \n");
        exit(EXIT_FAILURE);
    }
    printf("LISTENING SOCKET SETUP SUCCESSFUL \n");

    // Add listener to master set
    FD_SET(listener, &master_fds);

    // Keep track of the biggest file descriptor
    fdmax = listener;

    printf("WAITING FOR CLIENTS TO CONNECT... \n");
    while (1) {
        read_fds = master_fds;
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        // run through the existing connections looking for data to read
        for (int fd = 0; fd <= fdmax; ++fd) {
            if (FD_ISSET(fd, &read_fds)) {
                if (fd == listener) {
                    accept_new_connection(fd, &master_fds, fdmax, listener);
                }
            } else {
                handle_client_messages(fd, &master_fds, fdmax, buf, listener);
            }
        }
    }

    return 0;
}