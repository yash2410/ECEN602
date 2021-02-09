# ECEN 602 - Networking Assignment #1
## Description
In this assignment, a TCP client and server was implemented for a simple TCP echo service.
The server can handle up to 10 clients, and will echo back whatever each client sends to them.
Once the client has sent a message, it will be closed and the next client in the queue will be
polled for a message.  

## Architecture
### Server
The server uses IPV4 for the sockets. It starts off by setting up the socket and
binding the IP Address and port to the socket. It then begins listening on the 
unconnected socket for any clients that attempt to connect to it. Once a client
connects to the server, it will create a new thread for that client. Once a client
connects and sends a message, the server will echo back the message and stop the
thread that contained the client. Then it will look into its client queue for the 
next client. The server can hold up to 10 clients at once.

### Client
Client also uses IPV4 structure for sockets. The client setting up its socket and connecting
to the server. It takes the IP Address and port number as command line inputs. Once it is connected
to the server, it will be put in a queue of clients. It can send a message at any time, and once
the server reaches that client in the queue it will read the message and echo it back to 
the client. Once the client receives the message, it will shutdown the socket and close
the client.

## Build
To compile the server and client:
``` make all ```

To compile the server:
``` make echos ```

To compile the client:
``` make echo ```

To clean all object files:
``` make clean ```

## Usage
First start the server by typing the following in a terminal:
``` ./echos <PortNo> ```

Next start as many clients as you would like in separate terminal windows by typing the following:
```./echo <IPV4_Address> <PortNo>```


## Contributions
#### Yash Doshi
* Basic client with general sent and recv fucntions
* Basic Server which accpets a single client and echos the message

#### Parsa Dastjerdi
* Updated client to read in messages from stdin, along with other minor changes.
* Added threading to server to allow up to 10 clients to be used at once.