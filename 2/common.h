#include <stdio.h>
#include <stdlib.h>
#include <errno.h>		// defines perror(), herror()
#include <fcntl.h>		// set socket to non-blocking with fcntrl()
#include <unistd.h>
#include <string.h>
#include <assert.h>		//add diagnostics to a program
#include <netinet/in.h>		//defines in_addr and sockaddr_in structures
#include <arpa/inet.h>		//external definitions for inet functions
#include <netdb.h>		//getaddrinfo() & gethostbyname()
#include <sys/socket.h>		//
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <sys/select.h>		// for select() system call only
#include <sys/time.h>

//msg type
const struct sbcp_msg_type{
  const int JOIN = 2;
  const int FWD = 3;
  const int SEND = 4;
};

//msg attribute types
const struct sbcp_attribute_type{
  const int REASON = 1;
  const int USERNAME = 2;
  const int CLIENT_COUNT = 3;
  const int MSG = 4;
};

struct sbcp_attribute{
  int type:16;  //set to 16bits (2 byte)
  int length:16;
  char payload[512]; //4 Bytes
};

struct sbcp_message{
  int vrsn:9; //set to 9 bits
  int type:7; ///set to 7 bits
  int length:16;  
  struct sbcp_attribute msg_payload;
};
