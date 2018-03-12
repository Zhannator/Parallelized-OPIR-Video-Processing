/****************************************************************************************************************
This file contains the source code for a RSA encryption and decryption. It was written by Zhanneta Plokhovska 
(zhp3@pitt.edu) using some small pieces of RSA encryption and decryption code originally written by Soham Gandhi 
(https://www.codeproject.com/Articles/723175/Image-Cryptography-using-RSA-Algorithm-in-Csharp).
****************************************************************************************************************/

/* standard libraries */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define IMAGE_WIDTH 256
#define IMAGE_HEIGHT 256
#define DIM 65536 //IMAGE_WIDTH*IMAGE_HEIGHT

struct rsa_components_struct {
    uint16_t p; // prime number 1
    uint16_t q; // prime number 2
    uint16_t n; // n = p * q
    uint16_t phi; // phi = (p - 1) * (q - 1)
    uint16_t e; // 1 < e < phi, not factor of n
    uint16_t d; // (d * e) % phi = 1
};

/********************************************************
Used by big mod function. 
Returns a^2.
********************************************************/
uint16_t square(uint16_t a)
{
    return (a * a);
}

/********************************************************
Used by encryption and decryption functions. 
Returns b^p%m.
********************************************************/
uint16_t big_mod(uint16_t b, uint16_t p, uint16_t m) 
{
    if (p == 0)
        return 1;
    else if (p % 2 == 0)
        return square(big_mod(b, p / 2, m)) % m;
    else
        return ((b % m) * big_mod(b, p - 1, m)) % m;
}

/********************************************************
The following function does the actual encryption task. 
This function accepts the original frame and rsa 
components parameters. Each uint16_t in the frame is 
ciphered using RSA algorithm. Stores output in 
encrypted_frame.
********************************************************/
void rsa_encrypt_frame(uint16_t *original_frame, struct rsa_components_struct rsa_components, uint16_t *encrypted_frame)
{
    for (int i = 0; i < DIM; i++)
    {
        encrypted_frame[i] = big_mod(original_frame[i], rsa_components.e, rsa_components.n);
    }
    return;
}

/********************************************************
The following function does the actual decryption task. 
This function accepts the encrypted frame and rsa 
components parameters. Each uint16_t in the frame is 
deciphered using RSA algorithm. Stores output in
decrypted_frame.
********************************************************/
void rsa_decrypt_frame(uint16_t *encrypted_frame, struct rsa_components_struct rsa_components, uint16_t *decrypted_frame)
{
    for (int i = 0; i < DIM; i++)
    {
        decrypted_frame[i] = big_mod(encrypted_frame[i], rsa_components.d, rsa_components.n);
    }
    return;
}

/********************************************************
Used to generate rsa components needed for encryption
and decryption. Stores output in rsa_components.
********************************************************/
void rsa_generate_components(struct rsa_components_struct rsa_components) {   
    /* for testing purposes assign rsa components staticly */
    rsa_components.p = 3; 
    rsa_components.q = 11;
    rsa_components.n = rsa_components.p * rsa_components.q;
    rsa_components.phi = (rsa_components.p - 1) * (rsa_components.q - 1);
    rsa_components.e = 7;
    rsa_components.d = 3;
    return;   
}

int main(int argc, char **argv)
{
    char original_filename[50];
    if (argc > 1) {
        strcpy(original_filename, argv[1]);
    }
    
    char encrypted_filename[50] = "encrypted.raw";
    
    /* declare file variables */
	FILE *original_f;
	FILE *encrypted_f;
    
    /* open original file */
	printf("\nOpening original video file...\n");
	original_f = fopen(original_filename, "r");
	if (original_f == NULL){
		printf("\nError: Pointer to original_f is NULL.\n");
        exit(1);
	}
    
	/* calculate filesize, number of frames, and identify start and end pointers */
    printf("\nCalculating filesize, number of frames, and identifying start and end pointers...\n");
    printf("\nPointer of original_f = %p\n", original_f);
	long int position;
    fseek(original_f, 0, SEEK_END); //seek the end of file
	long const int filesize = ftell(original_f); //get the size of the file
	int frames = filesize/(DIM*2);
	printf("\nFrame count = %d\n", frames);
	position = ftell(original_f);
	printf("\nOffset of original_f pointer (end) = %ld\n", position);
	rewind(original_f); //set position back to beginning    
    position = ftell(original_f);
	printf("\nOffset of original_f pointer (start) = %ld \n", position);
    
    /* allocate memory for buffer to hold original video file */
    printf("\nAllocating memory for buffers...\n");
    uint16_t *original_buffer;
	original_buffer = (uint16_t*)malloc(sizeof(uint16_t)*(filesize));
	if (original_buffer == NULL){
		printf("Memory could not be allocated for the 16-bit original_buffer\n");
		exit(1);
	}
    
    /* read original file into original_buffer */
	printf("\nReading original_f...\n");
	fread(original_buffer, 2, filesize, original_f);
	printf("\nOffset of original_f pointer (after reading) = %p\n", original_f);    
    fclose(original_f);
    
    /* generate p, q, n, phi, e, and d for RSA encryption and decryption */
    struct rsa_components_struct rsa_components;
    rsa_generate_components(rsa_components);
    
    /* open encrypted file */
    printf("\nOpening output file...\n");
	encrypted_f = fopen(encrypted_filename, "w");
	if (encrypted_f == NULL){
		printf("\nError: Pointer to encrypted_f is NULL.\n");
        exit(1);
	}
    
    /* cycle through frames in original_buffer and perform operations on individual frames */
    printf("\nStarting operations on original_f...\n");
	for(int f = 0; f < frames; f++){
        printf("\nFrame number = %d\n", f);
        uint16_t *original_frame = &original_buffer[f*DIM];
        
        /* encrypt a frame */
        uint16_t *encrypted_frame;
        encrypted_frame = (uint16_t*)malloc(sizeof(uint16_t)*DIM);
        rsa_encrypt_frame(original_frame, rsa_components, encrypted_frame);
        
        /* write encrypted frame to output file */
        fwrite(encrypted_frame, DIM, 2, encrypted_f);

        /* decrypt a frame */
        uint16_t *decrypted_frame;
        decrypted_frame = (uint16_t*)malloc(sizeof(uint16_t)*DIM);
        rsa_decrypt_frame(encrypted_frame, rsa_components, decrypted_frame);
        
        //Compare original image to decrypted image
        for(int i = 0; i < DIM; i++){
            if (original_frame[i] != decrypted_frame[i]) {
                printf("\nDecrypted frame does not match the original frame.\n");
                exit(1);
            }
        }
        printf("\nOriginal frame and decrypted frame are identical.\n");
                
        free(encrypted_frame);
        free(decrypted_frame);
        
        break; //temporarily return after processing the first image, for testing
    }
    
    free(original_buffer);
    
    fclose(encrypted_f);
    
    return 0;
}
