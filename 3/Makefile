C = gcc

TARGETS = tftps

all: $(TARGETS)

tftps: tftps.o $(DEP)
	$(C) -o tftps tftps.o
	
clean:
	rm -f *.o tftps