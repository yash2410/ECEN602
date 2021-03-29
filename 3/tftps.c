#include "helpers.c"

/**
 * Notes:
 * You will want to create a working directory with your test files 
 * and server executable that is separate from your home directory so that 
 * someone cannot access your home directory via TFTP.
 */

 /** 
  * client_process psudeo code 
  *
  * recv from i(recv_fd) in select  
  * get opcode, filename etc
  * if opcode is (RRQ/ACK) follow (A/B)
  *************
  * A : _ rrq() in helpers.c_
  * create socket and add client info to client_ifo array 
  * send first block and wait for ACK
  * B : _ ack() in helpers.c_
  * ACK recieved 
  * Find client in client_array from sock_fd
  * send the next block
  */

struct client_info client_list[MAX_CLIENTS];

int client_process(int recv_fd){

  struct sockaddr_in client_addr;
  socklen_t client_len;
  char *buffer[MAX_PACKET_SIZE];
  
  //receive from recv_fd

  client_len = sizeof(client_addr);
  memset(buffer,0,sizeof(buffer));
  int recv_status = recvfrom(recv_fd,buffer,sizeof(buffer),0, (struct sockaddr *) &client_addr, &client_len);
  
  if (recv_status < 0){
    perror("recvfrom");
    exit(EXIT_FAILURE);
  }
  
  //get opcode

  uint16_t opcode;
  memset(&opcode,0,sizeof(opcode));
  memcpy(&opcode,buffer,sizeof(opcode));

  opcode = htons(opcode);

  switch (opcode)
  {
  case RRQ:
    printf("opcode : RRQ");
    rrq(buffer);
    break;
  case WRQ:
    printf("opcode : WRQ");
    break;
  case DATA:
    printf("opcode : DATA");
    break;
  case ACK:
    printf("opcode : ACK");
    ack(buffer,recv_fd);
    break;
  case ERROR:
    printf("opcode : ERROR");
    break;
  default:
    printf("Unidentified opcode : %d",opcode);
    break;
  }


}

int main(int argc, char *argv[])
{
  fd_set master_fds; // master file descriptor list
  fd_set read_fds;   // temp file descriptor list for select()
  int fdmax;         // max file descriptor number

  int opt = 1;
  int listener;
  struct sockaddr_in address;
  struct hostent *host;
  int nbytes;

  if (argc != 4)
  {
    perror("Usage: <./server server_ip server_port max_clients>");
    exit(EXIT_FAILURE);
  }

  char ip_address = IP_ADDRESS;  // argv[1];
  int port = PORT;               // atoi(argv[2]);
  int max_clients = MAX_CLIENTS; // atoi(argv[3]);

  FD_ZERO(&master_fds);
  FD_ZERO(&read_fds);

  if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == 0)
  {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }
  printf("[*] SOCKET SUCCESSFUL \n");

  if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
  {
    perror("server setsockopt() error");
    exit(EXIT_FAILURE);
  }
  printf("[*] SETSOCKOPT SUCCESSFUL \n");

  memset(&address, 0, sizeof(address));
  host = gethostbyname(ip_address);

  address.sin_family = AF_INET;
  memcpy(&address.sin_addr.s_addr, host->h_addr, host->h_length);
  address.sin_port = htons(port);

  if (bind(listener, (struct sockaddr *)&address, sizeof(address)) < 0)
  {
    perror("server bind() failed");
    exit(EXIT_FAILURE);
  }
  printf("[*] BIND SUCCESSFUL \n");

  FD_SET(listener, &master_fds);
  fdmax = listener;
  struct timeval tv = {10, 0};

   while (1)
  {
    int select_status = select(fdmax + 1, &read_fds, NULL, NULL, &tv);

    if (select_status == -1)
    {
      perror("select() failed");
      exit(EXIT_FAILURE);
    }

    else if (select_status == 0)
    {
      printf("Time Out Occured");
    }

    else
    {
      for (int fd = 0; fd <= fdmax; ++fd)
      {
        if (FD_ISSET(fd, &read_fds))
        {
          if (fd == listener)
          {
            /*
            * new_connection
            * call client_process() 
            * check client_fd < fdmax : (fdmax = client_fd)
            */
          }
        }
        else
        {
          /**
           * existent connection
           * call client_process()
          */
        }
      }
    }
  }
}