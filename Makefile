CC = gcc
CFLAGS = -Wall -g # -Werror -pedantic
LDFLAGS =

fetchmail: fetchmail.o
	$(CC) $(LDFLAGS) -o fetchmail fetchmail.o

fetchmail.o: fetchmail.c fetchmail.h
	$(CC) $(CFLAGS) -c fetchmail.c

clean:
	rm -f *.o fetchmail