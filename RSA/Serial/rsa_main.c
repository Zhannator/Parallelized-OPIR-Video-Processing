/****************************************************************************************************************
This file contains the source code for a RSA encryption and decryption. It was written by Zhanneta Plokhovska 
(zhp3@pitt.edu). It encrypts and decrypts raw video files (256 x 256 pixels frame size, 2 bytes per pixel) 
and compares original file to decrypted file.
****************************************************************************************************************/

/* standard c libraries */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <time.h>

/* other open source c libraries */
#include <gmp.h>

#define IMAGE_WIDTH 256
#define IMAGE_HEIGHT 256
#define DIM 65536 //IMAGE_WIDTH*IMAGE_HEIGHT

struct rsa_components_struct {
    mpz_t p; // prime number 1
    mpz_t q; // prime number 2
    mpz_t p_minus_1; // prime number 1
    mpz_t q_minus_1; // prime number 2
    mpz_t n; // n = p * q
    mpz_t phi; // phi = (p - 1) * (q - 1), must not share factor with e
    mpz_t e; // 2 < e < phi
    mpz_t d; // (d * e) % phi = 1
};

/********************************************************
This function performs encryption. It accepts encrypted 
frame and rsa parameters. Each uint16_t in the frame is 
ciphered using RSA algorithm. Stores output in
encrypted_frame.
********************************************************/
void rsa_encrypt_frame(uint16_t *original_frame, struct rsa_components_struct *rsa_components, unsigned int *encrypted_frame)
{
    /* using gmp library to deal with very large numbers */
    
    /* initialize gmp variables */
    mpz_t encrypted_pixel;
    mpz_t base;
    mpz_init(encrypted_pixel);
    mpz_init(base);
    
    for (int i = 0; i < DIM; i++)
    {
        /* set gmp variables */
        mpz_set_ui(encrypted_pixel, 0);
        mpz_set_ui(base, (unsigned int ) original_frame[i]);
        
        /* void mpz_powm (mpz_t rop, const mpz_t base, const mpz_t exp, const mpz_t mod) */
        /* Set rop to (base raised to exp) modulo mod. */
        mpz_powm(encrypted_pixel, base, rsa_components->e, rsa_components->n);

        /* convert back from mpz_t to uint16_t */
        encrypted_frame[i] = (unsigned int) mpz_get_ui(encrypted_pixel);
        
        /* Debugging
        if (i == 0) {
            gmp_printf ("encrypted_pixel is an mpz %Zd\n", encrypted_pixel);
            printf("\noriginal_frame[0] = %d\n", original_frame[i]);
            printf("\nencrypted_frame[0] = %d\n", encrypted_frame[i]);
        }
        */
    }
    
    /* free gmp variables */
    mpz_clear(encrypted_pixel);
    mpz_clear(base);
        
    return;
}

/********************************************************
This function performs decryption. It accepts encrypted 
frame and rsa parameters. Each uint16_t in the frame is 
deciphered using RSA algorithm. Stores output in
decrypted_frame.
********************************************************/
void rsa_decrypt_frame(unsigned int *encrypted_frame, struct rsa_components_struct *rsa_components, uint16_t *decrypted_frame)
{
    /* using gmp library to deal with very large numbers */
    
    /* initialize gmp variables */
    mpz_t dencrypted_pixel;
    mpz_t base;
    mpz_init(dencrypted_pixel);
    mpz_init(base);
    
    for (int i = 0; i < DIM; i++)
    {
        /* set gmp variables */
        mpz_set_ui(base, (unsigned int ) encrypted_frame[i]);
        
        /* void mpz_powm (mpz_t rop, const mpz_t base, const mpz_t exp, const mpz_t mod) */
        /* Set rop to (base raised to exp) modulo mod. */
        mpz_powm(dencrypted_pixel, base, rsa_components->d, rsa_components->n);

        /* convert back from mpz_t to uint16_t */
        decrypted_frame[i] = (uint16_t) mpz_get_ui(dencrypted_pixel);
        
        /* Debugging
        if (i == 0) {
            printf("\nencrypted_frame[0] = %d\n", encrypted_frame[i]);
            printf("\nencrypted_frame[0] = %d\n", decrypted_frame[i]);
        }
        */
    }
    
    /* free gmp variables */
    mpz_clear(dencrypted_pixel);
    mpz_clear(base);
        
    return;
}

/********************************************************
Used to generate rsa components needed for encryption
and decryption. Stores output in rsa_components.
********************************************************/
void rsa_generate_components(struct rsa_components_struct *rsa_components) {   
    /* for testing purposes assign rsa components statically */
    
    /* initialize gmp variables */
    mpz_init(rsa_components->p);
    mpz_init(rsa_components->q);
    mpz_init(rsa_components->p_minus_1);
    mpz_init(rsa_components->q_minus_1);
    mpz_init(rsa_components->n);
    mpz_init(rsa_components->phi);
    mpz_init(rsa_components->e);
    mpz_init(rsa_components->d);
    
    /* set/calculate gmp variables */
    mpz_set_ui(rsa_components->p, 52223);
    mpz_set_ui(rsa_components->q, 50833);
    mpz_mul(rsa_components->n, rsa_components->p, rsa_components->q);
    mpz_sub_ui(rsa_components->p_minus_1, rsa_components->p, 1);
    mpz_sub_ui(rsa_components->q_minus_1, rsa_components->q, 1);
    mpz_mul(rsa_components->phi, rsa_components->p_minus_1, rsa_components->q_minus_1);
    mpz_set_ui(rsa_components->e, 7);
    mpz_set_ui(rsa_components->d, 758442487);
    
    return;   
}

void rsa_free_components(struct rsa_components_struct *rsa_components) {   
    
    /* clear gmp variables */
    mpz_clear(rsa_components->p);
    mpz_clear(rsa_components->q);
    mpz_clear(rsa_components->p_minus_1);
    mpz_clear(rsa_components->q_minus_1);
    mpz_clear(rsa_components->n);
    mpz_clear(rsa_components->phi);
    mpz_clear(rsa_components->e);
    mpz_clear(rsa_components->d);
    
    return;   
}

/* 
    ./program filename.raw framerate(optional)
*/
int main(int argc, char **argv)
{
    char original_filename[50];
    float frame_freq = (float) 1.0/30.0; //default frame rate is 30 frames/sec
    
    printf("\nReading input arguments...\n");
    
    if (argc < 2) {
        printf("\nError: Invalid arguments. Please run the program as follows: ./program filename.raw\n");
        exit(0);
    }
        
    strcpy(original_filename, argv[1]);
    
    printf("\nVideo filename = %s\n", original_filename);

    
    if (argc > 2) {
        frame_freq = (float) 1.0/atof(argv[2]); //optionally user can provide his own framerate
    }
    
    printf("\nFrame frequency = %f sec\n", frame_freq);    
    
    char encrypted_filename[50] = "encrypted.raw";
    
    /* declare file variables */
	FILE *original_f;
	FILE *encrypted_f;
    
    /* open original file */
	printf("\nOpening original video file...\n");
	original_f = fopen(original_filename, "r");
	if (original_f == NULL) {
		printf("\nError: Pointer to original_f is NULL.\n");
        exit(1);
	}
    
	/* calculate filesize, number of frames, and identify start and end pointers */
    printf("\nCalculating filesize, number of frames, and identifying start and end pointers...\n");
    //printf("\nPointer of original_f = %p\n", original_f);
	long int position;
    fseek(original_f, 0, SEEK_END); //seek the end of file
	long const int filesize = ftell(original_f); //get the size of the file
	int frames = filesize/(DIM*2);
	printf("\nFrame count = %d\n", frames);
	position = ftell(original_f);
	//printf("\nOffset of original_f pointer (end) = %ld\n", position);
	rewind(original_f); //set position back to beginning    
    position = ftell(original_f);
	//printf("\nOffset of original_f pointer (start) = %ld \n", position);
    
    /* allocate memory for buffer to hold original video file */
    printf("\nAllocating memory for buffers...\n");
    uint16_t *original_buffer;
	original_buffer = (uint16_t*)malloc(sizeof(uint16_t)*(filesize));
	if (original_buffer == NULL) {
		printf("Memory could not be allocated for the 16-bit original_buffer\n");
		exit(1);
	}
    
    /* read original file into original_buffer */
	printf("\nReading original_f...\n");
	fread(original_buffer, sizeof(uint16_t), filesize, original_f);
	//printf("\nOffset of original_f pointer (after reading) = %p\n", original_f);    
    fclose(original_f);
    
    /* generate p, q, n, phi, e, and d for RSA encryption and decryption */
    struct rsa_components_struct rsa_components;
    rsa_generate_components(&rsa_components);
    
    /* open encrypted file */
    printf("\nOpening output file...\n");
	encrypted_f = fopen(encrypted_filename, "w");
	if (encrypted_f == NULL) {
		printf("\nError: Pointer to encrypted_f is NULL.\n");
        exit(1);
	}
    
    /* variables for calculating execution time and controlling frame rate */
    clock_t exec_start, exec_end, frame_start, frame_end;
    printf("\nStarting operations on original_f...\n");
    exec_start = clock(); //start counting execution time
    frame_start = clock();
    frame_end = frame_start;
    
    float delay = 0.0;
    
    /* cycle through frames in original_buffer and perform operations on individual frames */
	for (int f = 0; f < frames; f++) {
        /* Wait until next frame is available based on frame rate (frame_freq)*/
        clock_t current_time = clock();
        printf("\nExecution Time:   %0.6lf seconds\n", (float) current_time/CLOCKS_PER_SEC);
        while ((((float) (current_time - frame_start) / CLOCKS_PER_SEC)) <  frame_freq) {
            //printf("\nTime passed:   %0.6lf seconds\n", (float) (current_time - frame_start) / CLOCKS_PER_SEC);
            //printf("."); //Waiting on the next frame...\n");
            current_time = clock();
        };        
        
        frame_start = clock();
        delay = delay + (((float) (frame_end - frame_start) / CLOCKS_PER_SEC));
        
        printf("\nFrame number = %d\n", f);
        uint16_t *original_frame = &original_buffer[f*DIM];
        
        /* Debugging
        printf("\n**************************************\n");
        for(int i = 0; i < 10; i++) {
            printf("\noriginal_frame[%d] = %d\n", i, original_frame[i]);
        }
        printf("\n**************************************\n");
        */
        
        /* encrypt a frame */
        printf("\nEncrypting...\n");
        unsigned int *encrypted_frame;
        encrypted_frame = (unsigned int*)malloc(sizeof(unsigned int)*DIM);
        rsa_encrypt_frame(original_frame, &rsa_components, encrypted_frame);
        	
        /* write encrypted frame to output file */
        fwrite(encrypted_frame, DIM, sizeof(unsigned int), encrypted_f);

        /* decrypt a frame */
        printf("\nDecrypting...\n");
        uint16_t *decrypted_frame;
        decrypted_frame = (uint16_t*)malloc(sizeof(uint16_t)*DIM);
        rsa_decrypt_frame(encrypted_frame, &rsa_components, decrypted_frame);

        /* Debugging
        printf("\n**************************************\n");
        for(int i = 0; i < 10; i++) {
            printf("\ndecrypted_frame[%d] = %d\n", i, decrypted_frame[i]);
        }
        printf("\n**************************************\n");        
        */
        
        //Compare original image to decrypted image'
        printf("\nComparing original frame to decrypted frame...\n");
        for(int i = 0; i < DIM; i++){
            if (original_frame[i] != decrypted_frame[i]) {
                printf("\noriginal_frame[%d] = %d\n", i, original_frame[i]);
                printf("\ndecrypted_frame[%d] = %d\n", i, decrypted_frame[i]);
                printf("\nDecrypted frame does not match the original frame.\n");
                exit(1);
            }
        }
        printf("\nOriginal frame and decrypted frame are identical.\n");
                
        free(encrypted_frame);
        free(decrypted_frame);
        
        /* Capture when we got finished processing frame */
        frame_end = clock();
    }
    
    exec_end = clock(); //stop counting execution time
    printf("\nExecution Time:   %0.6lf seconds\n", (float) (exec_end - exec_start) / CLOCKS_PER_SEC);
    printf("\nTotal Delay Time:   %0.6lf seconds\n", (float) delay);
    
    rsa_free_components(&rsa_components);
    free(original_buffer);
    
    fclose(encrypted_f);
    
    return 0;
}
