#include "helpers.c"

#define IP_ADDRESS "127.0.0.1"
#define PORT 69
#define MAX_CLIENTS 5
#define MAX_PACKET_SIZE 516
#define BUFFER 1024

// Modes
#define NETASCII "NETASCII"
#define OCTET "OCTET"

/**
 * Notes:
 * You will want to create a working directory with your test files 
 * and server executable that is separate from your home directory so that 
 * someone cannot access your home directory via TFTP.
 */ 
int main(int argc, char *argv[]) {
    int listener;
    int opt = 1;
    struct sockaddr_in address;
    struct hostent* host;
    int client_addr_len;
    char buffer[BUFFER];

	pid_t child_pid;

    if (argc != 4) {
        perror("Usage: <./server server_ip server_port max_clients>");
        exit(EXIT_FAILURE);
    }

    char ip_address = IP_ADDRESS;  // argv[1];
    int port = PORT;               // atoi(argv[2]);
    int max_clients = MAX_CLIENTS; // atoi(argv[3]);

    memset(&address, 0, sizeof(address));
    host = gethostbyname(ip_address);

    address.sin_family = AF_INET;
    memcpy(&address.sin_addr.s_addr, host->h_addr, host->h_length);
    address.sin_port = htons(port);

    if ((listener = socket(AF_INET, SOCK_DGRAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    printf("[*] SOCKET SUCCESSFUL \n");

    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("server setsockopt() error");
        exit(EXIT_FAILURE);
    }
    printf("[*] SETSOCKOPT SUCCESSFUL \n");

    if (bind(listener, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("server bind() failed");
        exit(EXIT_FAILURE);
    }
    printf("[*] BIND SUCCESSFUL \n");
    
    /*
    while(1) {
        printf("Waiting for new client...\n");

        client_addr_len = sizeof(address);
        if ((new_fd, buffer, sizeof(buffer), 0) < 0) {
            
        }
    }

    shutdown(new_fd, SHUT_RDWR);
    close(new_fd);
    */
    return 0;
}