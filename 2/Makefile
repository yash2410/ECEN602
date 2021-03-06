C = gcc
DEP = common.h

TARGETS = sbcps sbcpc

all: $(TARGETS)

sbcps: server.o $(DEP)
	$(C) -o sbcp server.o

sbcpc: client.o $(DEP)
	$(C) -o sbcp client.o
	
clean:
	rm -f *.o sbcps sbcpc