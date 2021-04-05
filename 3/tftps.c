#include "helpers.c"
/**
 * UDP Server
 *********************
 * The request is sent by thr tftp client to the server
 * server receives the new request and checks for RRQ or WRQ
 * requests are sent to client_request() with all the client info to generate socket for the new client
 * the client is then sent to rrq or wrq with the said client_info.
 * in rrq the files is opened and checked if it exists then put into a for loop to send the file on the socket
 * till the last block, timeout after 3 retries
 */
/**
 * TODO 
 *********************
 * Implementing Netascii
 * Error and Ack (errors are not actually sent to tested for only the function is implemented)
 * Debugging
 * WRQ
 */
int main(int argc, char *argv[])
{
    int opt = 1;
    int listener;
    int rv;
    int numbytes;
    pid_t pid;
    struct addrinfo hints, *results, *p;

    // if (argc != 3)
    // {
    //   perror("Usage: <./server. server_ip server_port>");
    //   exit(EXIT_FAILURE);
    // }

    char ip_address = IP_ADDRESS;  // argv[1];
    int port = PORT;               // atoi(argv[2]);
    int max_clients = MAX_CLIENTS; // atoi(argv[3]);

    // -----------------------------------
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
        if ((listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("listener: socket");
            continue;
        }

        if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1) 
        {
            perror("server setsockopt() error");
            continue;
        }

        if (bind(listener, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(listener);
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
    printf("[*] LISTENER SOCKET SETUP SUCCESSFUL\n");
    freeaddrinfo(results);

    while (opt)
    {
        struct client_info client;

        int stat = 0;
        printf("listener: waiting to recvfrom...\n");

        client.size = recvfrom(listener, &client.buffer, sizeof(client.buffer), 0, (struct sockaddr *)&client.client, sizeof(client.client));
        int opcode = htons(client.buffer.opcode);

        switch (opcode)
        {
        case (RRQ || WRQ):
            pid = fork();
            if (pid < 0)
            {
                perror("fork()");
            }
            else
            {
                stat = client_request(client);
            }
            break;
        default:
            printf("Unidentified opcode : %d\n", opcode);
            break;
        }
        if (stat == -1)
        {
            printf("error in socket dropping client");
        }

        // not on the heap?
        // free(&client);
    }
}