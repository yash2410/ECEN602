#include "constants.c"

/**
 * Read Request Function
 */
int rrq(char buffer)
{
  printf("%s\n",buffer);
  printf("In RRQ\n");
}

/**
 * 
 */
int ack(char buffer, int recv_fd)
{
  printf("In ACK\n");
}

/**
 * BONUS: Write Request function
 */
int wrq()
{
}