#include "constants.h"

/**
 * Read Request Function
 * ----------------------------------------------------
 * 1. TFTP client sends RRQ to the TFTP server on chosen port number. Request contains
 *  a filename and transfer mode.
 * 
 * 2. TFTP server receives RRQ packet and creates a child process with fork(). Child process
 *  creates a new socket and calls bind() with a ephemeral port number. Child process
 *  then responds with directly with the first DATA packet, numbered starting from 1.
 *  All DATA packets contain a full-size block of 512 bytes, besides the last one.
 * 
 * 3. TFTP client receives first DATA packet and then determines the ephermal port number
 *  before responding. Child sends ACK with block number of 1 in response for first block.
 *  
 * 4. File transfer proceeds similar to the Stop-and-Wait protocol. TFTP server must receive
 *  a correctly numbered ACK before it can send the next DATA packet. The TFTP server is 
 *  responsible for setting a timeout timer and retransmitting the current DATA packet. 
 * 
 * 5. Final DATA packet must contain less than a full-sized block of data to signal that
 *  this is the last packet. Once an ACK is received for the final packet, the TFTP 
 *  child process should clean up and exit.
 */ 
int rrq() {
    
}

/**
 * BONUS: Write Request function
 */ 
int wrq() {

}

/**
 * Will return > 0 if the descriptor is readable.
 * -1 on error, 0 if timeout occurs, or positive value specifying the number of ready
 * descriptions (+1 in our case).
 * 
 * Server should count consecutive timeouts and then clean up and exit child process if
 * it reaches a certain amount. 
 */ 
int readable_timeo(int fd, int sec) {
    fd_set rset;
    struct timeval tv;

    FD_ZERO(&rset);
    FD_SET(fd, &rset);

    tv.tv_sec = sec;
    tv.tv_usec = 0;

    return (select(fd + 1, &rset, NULL, NULL, &tv));
}

/*
int netascii() {
    c = getc(fp);

    if (c == EOF) {
        if (ferror(fp)) {
            err_dump("read err from getc on local file");
        }
        return count;
    } else if (c == '\n') {
        c = '\r';
        nextchar = '\n';
    } else if (c == '\r') {
        nextchar = '\0';
    } else {
        nextchar = -1;
    }
}
*/