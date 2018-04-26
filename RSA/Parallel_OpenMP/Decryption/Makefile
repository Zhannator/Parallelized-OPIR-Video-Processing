CC := mpicc
CFLAGS += -std=c99 -lgmp -Wall -g -fopenmp
LDFLAGS += -lm

rsa_decryption_main: rsa_decryption_main.c
    $(CC) -o $@ $(CFLAGS) $< $(LDFLAGS)

clean:
    $(RM) -f rsa_decryption_main
    
.PHONY: clean