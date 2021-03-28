C = gcc

TARGETS = tftps

all: $(TARGETS)

tftps: server.o $(DEP)
	$(C) -o tftps server.o
	
clean:
	rm -f *.o tftps