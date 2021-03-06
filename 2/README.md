## ECEN 602 Network Programming Assignment #2
The Simple Broadcast Chat Protocol is a protocol that allows clients to:
    1. Join or leave a global chat session
    2. View members of the session
    3. Send and receive messages

A single instance of the server provides a single "chat room" that will handle a finite number of clients.

    1. Clients will receive a list of the connected members of the chat session once they complete the JOIN transaction. 
    2. Clients use SEND messages to carry chat text, and clients receive chat text from the server using the FWD message.
    3. Clients may exit uneremoniously at any time during the chat session.
    4. The server should detect a client exit, cleanup the resources allocated to that client and notify the other clients. 
    5. The client will be able to detect idle clients and notify the server.

