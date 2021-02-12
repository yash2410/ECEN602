#include"common.h"

int main(int argc, char *argv[]){

    struct sockaddr_in addr_server;
    int cfd;
    fd_set read_fd;
    char username[16];

    if (argc != 3){
        perror("Usage: echo <IPAddress> <Port>\n");
        exit(EXIT_FAILURE);
    }

    memset(username, '\0', sizeof(username));
    printf("Enter username :\n");
    fgets(username,16,stdin); //fgets to limit id to 16

    char* ip_addr = argv[1];
    int port = atoi(argv[2]);

    /* ---- Set up client ------ */
    // Initialize socket using TCP
    if ((cfd = socket(AF_UNSPEC, SOCK_STREAM, 0)) < 0){ //IP agnostic
        perror("client socket() error");
        exit(EXIT_FAILURE);
	};
    printf("[+] SOCKET CREATED \n");

    addr_server.sin_family = AF_UNSPEC;
    addr_server.sin_port = htons(port);
  
    // Convert from ASCII string to binary network address
    //AF_INET here because tamu ece servers block IPv6
    if (inet_pton(AF_INET, ip_addr, &addr_server.sin_addr) <= 0){  
        perror("client inet_pton() error"); 
        exit(EXIT_FAILURE);
    } 
  
    // Connect to the server
    if (connect(cfd, (struct sockaddr *)&addr_server, sizeof(addr_server)) < 0){
        perror("client connect() error");
        exit(EXIT_FAILURE);
    } 
    printf("[+] CONNECTED TO SERVER \n");

    // TODO : Join server with username authentication

    printf("[+] USER %s AUTHENTICATED",username);
    
    FD_ZERO(&read_fd);
    FD_SET(0,&read_fd);
    FD_SET(cfd,&read_fd);


    /*
    iterarte through all fd in fd_set and check for any event in an infinite loop.
    0 --> send to server
    cfd --> revcieve from server  
    and ignores anyother fd because it is a client and doesnt have to connect to anyone else
    */
    while(1){
      
      if(select(cfd+1,&read_fd,NULL,NULL,NULL)<0){
        perror("client select() error");
        exit(EXIT_FAILURE);
      }
      else{
        
        if(FD_ISSET(cfd,&read_fd)){
          printf("recieve stuff");
        } 

        if(FD_ISSET(0,&read_fd)){
          printf("send stuff");
        }

      }

    }
    
    return 0;
}