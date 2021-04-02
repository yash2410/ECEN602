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

  pid_t child_pid;

  int opt = 1;
  int proc_stat;
  int listener;
  int recv_status;
  int nbytes;
  pid_t pid;
  struct sockaddr_in address;
  // if (argc != 3)
  // {
  //   perror("Usage: <./server. server_ip server_port>");
  //   exit(EXIT_FAILURE);
  // }

  char ip_address = IP_ADDRESS;  // argv[1];
  int port = PORT;               // atoi(argv[2]);
  int max_clients = MAX_CLIENTS; // atoi(argv[3]);

  if ((listener = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == 0)
  {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }
  printf("[*] SOCKET SUCCESSFUL \n");

  int stat = setsockopt(listener, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
  if (stat < 0)
  {
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

  while (opt)
  { 
    struct client_info client;

    int stat = 0;
    printf("Waiting for client\n");

    client.size = recvfrom(listener, &client.buffer, sizeof(client.buffer), 0, (struct sockaddr *)&client.client, sizeof(client.client));
    int opcode = htons(client.buffer.opcode);
    pid = fork();
    if (pid < 0)
    {
      perror("fork()");
    }
    else
    {
      switch (opcode)
      {
      case (RRQ || WRQ):
        stat = client_request(client);
        break;
      default:
        printf("Unidentified opcode : %d\n",opcode);
        break;
      }
    }
    if(stat = -1){
      printf("error in socket dropping client");
    }
    free(&client);
  }
}