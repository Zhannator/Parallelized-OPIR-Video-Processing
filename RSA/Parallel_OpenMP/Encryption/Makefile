CC := gcc
CFLAGS += -std=c99 -lgmp -Wall -g -fopenmp
LDFLAGS += -lm

rsa_encryption_main: rsa_encryption_main.c
    $(CC) -o $@ $(CFLAGS) $< $(LDFLAGS)

clean:
    $(RM) -f rsa_encryption_main
    
.PHONY: clean