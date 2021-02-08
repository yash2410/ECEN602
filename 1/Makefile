C = gcc
DEP = common.h

TARGETS = echos echo

all: $(TARGETS)

echos: server.o $(DEP)
	$(C) -o echos server.o

echo: client.o $(DEP)
	$(C) -o echo client.o
	
clean:
	rm -f *.o *~ echos echo