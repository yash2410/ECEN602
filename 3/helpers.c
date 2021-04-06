#include "constants.c"

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
    int numbytes = sendto(client.sock, &data, sizeof(&data), 0, (struct sockaddr *)&client.client, sizeof(client.client)) < 0)
    if (numbytes == -1)
    {
        perror("server data.sendto error()");
    }
}

/**
 * Error
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
 * Read Request Function
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
