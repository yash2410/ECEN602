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
#define BUFFER_SIZE 256

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
    int yes = 1;    // For setsockopt() SO_REUSEADDR, below
    int rv;

    struct addrinfo hints, *ai, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "poll server: %s \n", gai_strerror(rv));
        exit(EXIT_FAILURE);
    }

    for (p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            continue;
        }

        // Get rid of "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    // Done with this address info
    freeaddrinfo(ai);

    if (listen(listener, 10) == -1) {
        return -1;
    }

    // Didn't get bound
    if (p == NULL) {
        fprintf(stderr, "select server: failed to bind \n");
        exit(EXIT_FAILURE);
    }

    return listener;
}

int main(int argc, char* argv[]) {
    fd_set master_fds;  // master file descriptor list
    fd_set read_fds;    // temp file descriptor list for select()
    int fdmax;          // max file descriptor number

    int newfd;
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen;

    char buf[BUFFER_SIZE];
    int nbytes;
    char remoteIP[INET6_ADDRSTRLEN];

    FD_ZERO(&master_fds);
    FD_ZERO(&read_fds);

    int listener = get_listener_socket();

    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // add listener to master set
    FD_SET(listener, &master_fds);

    // keep track of the biggest file descriptor
    fdmax = listener;

    while (1) {
        read_fds = master_fds;
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        // run through the existing connections looking for data to read
        for (int i = 0; i <= fdmax; ++i) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == listener) {
                    // handle new connections
                    addrlen = sizeof(remoteaddr);
                    newfd = accept(listener, (struct sockaddr*)&remoteaddr, &addrlen);
                    if (newfd == -1) {
                        perror("accept");
                    }

                } else {
                    FD_SET(newfd, &master_fds);
                    if (newfd > fdmax) {
                        fdmax = newfd;
                    }
                    printf("select server: new connection from %s on socket %d \n", 
                        inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*) &remoteaddr),
                        remoteIP, INET6_ADDRSTRLEN), newfd);
                }

            } else {
                // handle data from a client
                if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0) {
                    // got error or connection closed by client
                    if (nbytes == 0) {
                        printf("select server: socket %d hung up \n", i);
                    } else {
                        perror("recv");
                    }

                    close(i);
                    FD_CLR(i, &master_fds); // remove from master set

                } else {
                    // we got the some data from a client
                    if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0) {
                            printf("select server: socket %d hung up \n", i);
                        } else {
                            perror("recv");
                        }
                        close(i);
                        FD_CLR(i, &master_fds);

                    } else {
                        for (int j = 0; j <= fdmax; ++j) {
                            // send to everyone
                            if (FD_ISSET(j, &master_fds)) {
                                // except the listener and ourselves
                                if (j != listener && j != i) {
                                    if (send(j, buf, nbytes, 0) == -1) {
                                        perror("send");
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}