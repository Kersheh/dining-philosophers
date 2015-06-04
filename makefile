CC = gcc
CFLAGS = -g -Wall

all: dine

dine: dine.c
	$(CC) $(CFLAGS) -o dine dine.c -lpthread

clean:
	$(RM) *.o dine holes