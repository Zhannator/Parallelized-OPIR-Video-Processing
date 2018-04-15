CC := gcc
CFLAGS += -std=c99 -lgmp

all: rsa_main rsa_encryption_main rsa_decryption_main rsa_compare_main

rsa_main: rsa_main.c
    $(CC) -o $@ $(CFLAGS) $<

rsa_encryption_main: rsa_encryption_main.c
    $(CC) -o $@ $(CFLAGS) $<

rsa_decryption_main: rsa_decryption_main.c
    $(CC) -o $@ $(CFLAGS) $<

rsa_compare_main: rsa_compare_main.c
    $(CC) -o $@ $(CFLAGS) $<

clean:
    $(RM) -f rsa_main
    $(RM) -f rsa_encryption_main
    $(RM) -f rsa_decryption_main
    $(RM) -f rsa_compare_main
    
.PHONY: clean