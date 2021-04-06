# TFTP Server
In this assignment a TFTP server was implemented. The server uses UDP to send files from 
TFTP clients to our TFTP server, and uses a Stop-And-Wait protocol to send an entire file. 

## Architecture
The server starts off by creating the listener socket, that will listen for incoming 
connections. The server will listen for incoming client packets and 
then create a fork with a new socket for each new client. Once a socket has been created
for each new client, messages can be sent back and forth. Files are broken up into
blocks of size 512 bytes. Each file is split block by block and sent to the client 
in individual blocks. Once a block that is less than 512 bytes is sent, the transmission
stops and the ephemeral socket is closed. 

## Usage
Build the server using:
``` make ```

Run the server using:
``` ./tftps <ip_address> <port_number> ```

## Roles
### Yash Doshi
1. Set up server and implemented functionality to fork new sockets
2. Wrote RRQ method to read data from local machine
3. Implemented method for sending data and sending error messages

### Parsa Dastjerdi
1. Implemented netascii and octet reads
2. Added error messages throughout TFTP server
3. Implemented timeout method