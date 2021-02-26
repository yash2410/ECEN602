#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/time.h>

//msg type
#define JOIN 2
#define FWD 3
#define SEND 4

//attribute type
#define REASON 1
#define USERNAME 2
#define CLIENT_COUNT 3
#define MSG 4

struct sbcp_attribute
{
  int type : 16; //set to 16bits (2 byte)
  int length : 16;
  char payload[512]; //4 Bytes
};

struct sbcp_message
{
  int vrsn : 9; //set to 9 bits
  int type : 7; ///set to 7 bits
  int length : 16;
  struct sbcp_attribute msg_payload;
};

int writen(int socketfd, char *buffer)
{
  int err = send(socketfd, buffer, strlen(buffer), 0);
  if (err < 0)
  {
    perror("send() error");
    exit(EXIT_FAILURE);
  }
  printf("[+] MSG SENT: %s\n", buffer);

  return err;
}

/**
 * Will read a line of characters from a socket and return the number of bytes
 * read or a -1 on error.  
 **/
int readline(int socketfd, char *buffer, int numBytes)
{
  int err = recv(socketfd, buffer, sizeof(buffer), 0);
  if (err < 0)
  {
    perror("recv() error");
    exit(EXIT_FAILURE);
  }
  printf("[+] MSG RECIEVED: %s\n", buffer);

  return err;
}
char msg_[1000];

//convert structered message to sendable string
char *pack(struct sbcp_message msg)
{
  memset(&msg_, 0, sizeof(msg_));
  sprintf(msg_, "%d:%d:%d:%d:%d:%s", msg.vrsn, msg.type, msg.length, msg.msg_payload.type, msg.msg_payload.length, msg.msg_payload.payload);
  return (char *)msg_;
}

//convert recv string in to structured message
struct sbcp_message unpack(char msg)
{

  struct sbcp_message msg_u;
  char *msg_vrsn, msg_type, msg_len, msg_attribute_type, msg_attribute_len, msg_attribute_payload[511];

  sscanf(msg, "%[^:]:%[^:]:%[^:]:%[^:]:%[^:]:%s", &msg_vrsn, &msg_type, &msg_len, &msg_attribute_type, &msg_attribute_len, msg_attribute_payload);

  memset(&msg, 0, sizeof(msg_u));

  msg_u.vrsn = msg_vrsn - '0';
  msg_u.type = msg_type - '0';
  msg_u.length = msg_len - '0';
  msg_u.msg_payload.type = msg_attribute_type - '0';
  msg_u.msg_payload.length = msg_attribute_len - '0';

  for (int i = 0; i < sizeof(msg_attribute_payload); i++)
  {
    msg_u.msg_payload.payload[i] = msg_attribute_payload[i];
  }

  return msg_u;
}

#endif