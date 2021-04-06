#include "constants.c"

/**
 * Standard octet transmission
 * what is fp??
 */ 
int octet_rrq(char *buf, char *fp, int MAXBUFLEN) {
    // size_t numbytes = fread(buf, 1, MAXBUFLEN, *fp);
    int numbytes = 0;
    return numbytes;
}

int octet_wrq() {
    return 0;
}


/**
 * Standard netascii transmission
 * nextchar needs to be global
 * returns the nextchar
 * TODO: what is fp
 */ 
char netascii_rrq(char *buf, int maxnbytes, char fp) {
    char c, *ptr;
    char nextchar = buf;

    for (int count = 0; count < maxnbytes; ++count) {
        if (nextchar >= 0) {
            *ptr++ = nextchar;
            nextchar = -1;
            continue;
        }

        c = getc(fp);

        if (c == EOF) {
            if (ferror(fp)) {
                // err_dump("read err from getc on local file");
                perror("read err from getc on local file");
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

        *ptr++ = c;
    }

    return nextchar;
}

int netascii_wrq() {
    return 0;
}


/**
 * Timeout function for sending data
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


/**
 * Includes functions for sending and receiving requests.
 * client request : takes client_info generates a socket specific to the client and sorts into rrq or wrq
 * rrq : gets the filename and data to send to the client as well as handles ACKs
 * error : sends error to the clients
 * send_data : sends data packets on request from client
 */
void send_data(struct data data, struct client_info client)
{
    data.opcode = htons(DATA);
    int numbytes = sendto(client.sock, &data, sizeof(&data), 0, (struct sockaddr *)&client.client, sizeof(client.client));
    if (numbytes == -1)
    {
        perror("server data.sendto error()");
    }
    printf("sent data to client\n");
}


/**
 * TFTP error messages will be sent if the a RRQ or WRQ message cannot be processed
 * RRQ or WRQ errors during file transmission can also cause this message to be sent,
 * and transmission is then terminated.
 */
void error(struct client_info client, char error_string)
{
    struct error error;
    error.opcode = htons(ERROR);
    error.error_num = 0;
    int numbytes = sendto(client.sock, &error, sizeof(&error), 0, (struct sockaddr *)&client.client, sizeof(client.client));
    if (numbytes == -1)
    {
        perror("server error.sendto error()");
    }
}

/**
 * Ack for WRQ
 */
void ack() {

}


/**
 * Read Request Function
 * TODO: add octet and netascii transmission
 * mode_s holds octet and netascii transmission type
 */
void rrq(struct client_info client)
{
    char *filename = client.buffer.file;
    char *mode_s = strchr(filename, '\0') + 1;
    int block_num = 0;
    FILE *fd = fopen(fopen, "r");
    int close = 0;

    if (fd == NULL)
    {
        error(client, "file not found");
    }

    struct data data_;

    while (!close)
    {
        int data_len = fread(data_.data, 1, sizeof(data_.data), fd);
        block_num++;

        if (data_len < sizeof(data_.data))
        {
            close = 1;
        }

        data_.block_number = htons(block_num);
        // send data to client
        send_data(data_, client);

        // get back the ACK
        client.size = recvfrom(client.sock, &client.buffer, sizeof(client.buffer), 0, (struct sockaddr *)&client.client, sizeof(client.client));
        if (client.size < 0)
        {
            perror("rrq_recvfrom");
        }
        printf("ACK for block number %u", data_.block_number);
    }
}


/**
 * BONUS: Write Request function
 */
void wrq(struct client_info client)
{
}

/**
 * Client Request is a wrapper function that will either call RRQ or WRQ
 * based on the opcode. 
 * After the fork is created, it will call either RRQ or WRQ based on the opcode
*/
int client_request(struct client_info client)
{
    int opcode = htons(client.buffer.opcode);
    int sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == -1) 
    {
        perror("client_request sock() error");
        return -1;
    }

    if (bind(client.sock, (struct sockaddr *)&client.client, sizeof(client.client)) == -1)
    {
        perror("client_request bind() error");
        return -1;
    }

    client.sock = sock;

    if (opcode == RRQ)
    {
        printf("RRQ packet\n");
        rrq(client);
    }
    else if (opcode == WRQ)
    {   
        printf("WRQ packet\n");
        wrq(client);
    }

    pclose(sock);
    return 0;
}
