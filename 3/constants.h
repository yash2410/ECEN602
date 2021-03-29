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

enum tftp_opcode {
    RRQ     = 1,
    WRQ     = 2,
    DATA    = 3,
    ACK     = 4,
    ERROR   = 5
};

// Max size of all packets (barring ACK) is 516 bytes
union packets {
    struct RRQ_WRQ {
        uint16_t opcode;
        char filename[502];
        uint8_t f1; //set it to 0
        char mode[10];
        uint8_t f2; //set it to 0
    };

    struct WRQ {
        uint16_t opcode;
        char filename[502];
        uint8_t f1; // set to 0
        char mode[10];
        uint8_t f2; // set to 0
    };

    struct DATA {
        uint16_t opcode;
        uint16_t block_number;
        char data[512];
    };

    struct ACK {
        uint16_t opcode;
        uint16_t block_number;
    };

    struct ERROR {
        uint16_t opcode;
        uint16_t error_num;
        char data[511];
        uint8_t f1; //set it to 0
    };
};

#endif