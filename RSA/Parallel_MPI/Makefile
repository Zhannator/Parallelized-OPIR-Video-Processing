CC := mpicc
CFLAGS += -std=c99
LDFLAGS += -lm


rsa_encryption_main: rsa_encryption_main.c
    $(CC) -o $@ $(CFLAGS) $< $(LDFLAGS)

clean:
    $(RM) -f rsa_encryption_main
    
.PHONY: clean