target  = FullToEvenOdd
sources = main.c $(target).c
objects = $(patsubst %.c,%.o,$(sources))
CC = gcc
OPTS = -Wall

all: $(objects)
	$(CC) $(OPTS) -o $(target) $(objects)

install:
	cp $(target) /usr/local/bin

clean:
	$(RM) $(target) $(objects)
