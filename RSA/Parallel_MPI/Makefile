CC := mpicc
CFLAGS += -std=c99 -lgmp -Wall -g
LDFLAGS += -lm

all: rsa_encryption_main rsa_decryption_main rsa_compare_main

rsa_decryption_main: rsa_decryption_main.c
    $(CC) -o $@ $(CFLAGS) $< $(LDFLAGS)

rsa_encryption_main: rsa_encryption_main.c
    $(CC) -o $@ $(CFLAGS) $< $(LDFLAGS)

rsa_compare_main: rsa_compare_main.c
    gcc -o $@ $(CFLAGS) $<
    
clean:
    $(RM) -f rsa_encryption_main
    $(RM) -f rsa_decryption_main
    $(RM) -f rsa_compare_main
    
.PHONY: clean