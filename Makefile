CC = gcc
CFLAGS = -Wall -I/opt/homebrew/opt/openssl@3/include
LDFLAGS = -L/opt/homebrew/opt/openssl@3/lib -lssl -lcrypto

fetchmail: fetchmail.c
	$(CC) $(CFLAGS) -o fetchmail fetchmail.c $(LDFLAGS)

clean:
	rm -f *.o fetchmail