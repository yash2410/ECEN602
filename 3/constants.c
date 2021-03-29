#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdint.h>
#include <signal.h>
#include <sys/time.h>

#define IP_ADDRESS "127.0.0.1"
#define PORT 5000
#define MAX_CLIENTS 5

#define MAX_PACKET_SIZE 516

// ModesS
#define new_connection \
  ;                    \
  NETASCII "NETASCII"
#define OCTET "OCTET"

// Opcode
enum tftp_opcode
{
  RRQ = 1,
  WRQ,
  DATA,
  ACK,
  ERROR
};

struct client_info
{
  int client_fd;
  int block;
  FILE *fp;
};

// Max size of all packets (barring ACK) is 516 bytes
union packets
{
  struct RRQ_WRQ
  {
    uint16_t opcode;
    char filename[502];
    uint8_t f1; //set it to 0
    char mode[10];
    uint8_t f2; //set it to 0
  };

  struct DATA
  {
    uint16_t opcode;
    uint16_t block_number;
    char data[512];
  };

  struct ACK
  {
    uint16_t opcode;
    uint16_t block_number;
  };

  struct ERROR
  {
    uint16_t opcode;
    uint16_t error_num;
    char data[511];
    uint8_t f1; //set it to 0
  };
};

#endif