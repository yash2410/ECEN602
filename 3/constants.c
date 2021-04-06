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
#include <time.h>

#define IP_ADDRESS "127.0.0.1"
#define PORT "8000"
#define MAX_CLIENTS 5

#define MAX_PACKET_SIZE 516

#define MAX_RETRIES 3;
// ModesS
#define NETASCII "NETASCII"
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

// Max size of all packets (barring ACK) is 516 bytes

struct rrq_wrq
{
  uint16_t opcode;
  uint8_t file;
};

struct data
{
  uint16_t opcode;
  uint16_t block_number;
  uint8_t data[512];
};

struct ack
{
  uint16_t opcode;
  uint16_t block_number;
};

struct error
{
  uint16_t opcode;
  uint16_t error_num;
  uint8_t error[512];
};

// Client Info hold all the information of the client i.e the files, socket, address and size
struct client_info
{
  int size;
  struct rrq_wrq buffer;
  struct sockaddr_in client;
  int sock;
};

#endif