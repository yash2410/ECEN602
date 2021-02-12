#include "common.h"

/*
Client for using SBCP Chat Protocol
to run : client  <server_ip> <port> 

client connects to the server using msg type join and authenticating with the usernames.
The socket listens for events to recieve from the server(FWD messages from other clients)
and send messages to the server.
*/

int main(int argc, char *argv[])
{

  struct sockaddr_in addr_server;
  int cfd;
  fd_set read_fd;
  int fd_max;
  char username[16];

  if (argc != 3)
  {
    perror("Usage: echo <IPAddress> <Port> \n");
    exit(EXIT_FAILURE);
  }

  memset(username, 0, sizeof(username));
  printf("Enter username : \n");
  fgets(username, 16, stdin);

  char *ip_addr = argv[1];
  int port = atoi(argv[2]);

  if ((cfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("client socket() error");
    exit(EXIT_FAILURE);
  };
  printf("[+] SOCKET CREATED \n");

  addr_server.sin_family = AF_INET;
  addr_server.sin_port = htons(port);

  // Convert from ASCII string to binary network address
  if (inet_pton(AF_INET, ip_addr, &addr_server.sin_addr) <= 0)
  {
    perror("client inet_pton() error");
    exit(EXIT_FAILURE);
  }

  // Connect to the server
  if (connect(cfd, (struct sockaddr *)&addr_server, sizeof(addr_server)) < 0)
  {
    perror("client connect() error");
    exit(EXIT_FAILURE);
  }
  printf("[+] CONNECTED TO SERVER \n");

  //TODO : autenticate with server by sending username

  FD_ZERO(&read_fd);
  FD_SET(cfd,&read_fd);
  FD_SET(0,&read_fd);

}