CC := gcc
CFLAGS += -std=c99 -lgmp

rsa_main: rsa_main.c
	$(CC) -o $@ $(CFLAGS) $<

clean:
	$(RM) -f rsa_main
