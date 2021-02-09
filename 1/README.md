# ECEN 602 - Networking Assignment #1
## Description
In this assignment, a TCP client and server was implemented for a simple TCP echo service.
The server can handle up to 10 clients, and will echo back whatever each client sends to them.
Once the client has sent a message, it will be closed and the next client in the queue will be
polled for a message.  

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
``` .echos <PortNo> ```

Next start as many clients as you would like in separate terminal windows by typing the following:
```./echo <IPV4_Address> <PortNo>```

## Testbench
[Link to Testbench report](https://docs.google.com/document/d/1zmjA_fEX7kTE6ZplxLcg_hn4Qkjna99N3--LW4ojmJY/edit?usp=sharing)
## Contributions
#### Yash Doshi
* Basic client with general sent and recv fucntions
* Basic Server which accpets a single client and echos the message

#### Parsa Dastjerdi
* Updated client to read in messages from stdin, along with other minor changes.
* Added threading to server to allow up to 10 clients to be used at once.
