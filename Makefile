IDIR = ../include
CC = arm-linux-gnueabihf-gcc

CFLAGS = -I.
CFLAGS += -I$(IDIR)

ODIR = obj
LDIR = ../lib

LIBS = 

_DEPS = tty.h
DEPS = $(patsubst %, $(IDIR)/%, $(_DEPS))

_OBJ = tty.o test.o
OBJ = $(patsubst %, $(ODIR)/%, $(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

test: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -rf $(ODIR)/*.o *~ test
