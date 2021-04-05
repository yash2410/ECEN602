#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

/**
 * Sits on a machine waiting for an incoming packet on port 4950.
 * talker sends a packet to that port, on the specified machine, that contains whatever the
 * user enters on the command line.
 */

#define PORT "4950"
#define MAXBUFLEN 100

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main()
{
    int sockfd;
    struct addrinfo hints, *results, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage client_addr;
    char buf[MAXBUFLEN];
    socklen_t client_addr_len;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use local machine's IP address

    if ((rv = getaddrinfo(NULL, PORT, &hints, &results)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first one we can;
    for (p = results; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "listener: failed to bind socket \n");
        return 2;
    }

    freeaddrinfo(results);
    printf("listener: waiting to recvfrom...\n");

    while (1)
    {
        client_addr_len = sizeof(client_addr);
        if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN - 1, 0, (struct sockaddr *)&client_addr, &client_addr_len)) == -1)
        {
            perror("recvfrom");
            exit(1);
        }

        printf("listener: got packet from %s\n",
               inet_ntop(client_addr.ss_family,
                         get_in_addr((struct sockaddr *)&client_addr),
                         s,
                         sizeof(s)));

        printf("listener: packet is %d bytes long\n", numbytes);
        buf[numbytes] = '\0';
        printf("listener: packet contains \"%s\"\n", buf);
    }

    close(sockfd);
    return 0;
}