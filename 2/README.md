# ECEN 602 - Networking Assignment #2
## Description
In this assignment, a TCP client and server was implemented for a simple TCP echo service.
The server can handle up to 10 clients, and will echo back whatever each client sends to them.
Once the client has sent a message, it will be closed and the next client in the queue will be
polled for a message.  

In this assignment, a simple broadcast chat application was implemented. The server uses the 
select() statement to handle multiple clients, instead of forking off a new process for each
incoming client. Once a client has sent a message, that message is broadcast to all the other 
clients currently connected to the server.

## Architecture
### Server
The server begins by creating a listening socket to listen for new clients. Once it begins
listening, continuously loop and accept clients. Once a client connects to a 
server, it will either broadcast its message or be added to the list of clients. The server
will loop and check if anything is on the listening port, if there is nothing, then it will 
handle any client messages that have been queued up. If there is a new client on the listening
port, then it will be added to the list of clients. If there is nothing on the listening port,
the next message in the queue will be broadcast to the rest of the clients. 


### Client
The client begins by spinning up its own listening socket, and then attempting to connect 
to the server. Once it is connected to the server, it will be able to send messages to the
server that will then be broadcast to the rest of the clients currently connected to the 
server. 


## Build
To compile the server and client:
``` make all ```

To compile the server:
``` make server ```

To compile the client:
``` make client ```

To clean all object files:
``` make clean ```

## Usage
First start the server by typing the following in a terminal:
``` <./sbcps server_ip server_port max_clients> ```

Next start as many clients as you would like in separate terminal windows by typing the following:
```<./sbcpc client_ip client_port>```

## Testbench
[Link to Testbench report](https://docs.google.com/document/d/1zmjA_fEX7kTE6ZplxLcg_hn4Qkjna99N3--LW4ojmJY/edit?usp=sharing)

## Contributions
#### Yash Doshi
* Wrote client application
* Updated functions in common.h for this assignment

#### Parsa Dastjerdi
* Wrote server application
