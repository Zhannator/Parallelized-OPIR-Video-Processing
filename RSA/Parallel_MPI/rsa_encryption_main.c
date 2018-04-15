/****************************************************************************************************************
This file contains the source code for a RSA encryption. It was written by Zhanneta Plokhovska 
(zhp3@pitt.edu). It encrypts raw video files (256 x 256 pixels frame size, 2 bytes per pixel).
****************************************************************************************************************/

/* standard c libraries */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <time.h>

/* standard mpi libraries */
#include <mpi.h>

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
    ./program num_of_frames filename.raw framerate(optional)
*/
int main(int argc, char **argv)
{
    char original_filename[50];
    float frame_freq;
    uint16_t *original_buffer;
    unsigned int *encrypted_buffer;
    struct rsa_components_struct rsa_components;
    clock_t exec_start, exec_end, frame_start, frame_end;
    float delay;
	int frames;
    FILE *original_f;
	FILE *encrypted_f;
    
    /* MPI variables */
    int rank; // id of current node
    int size; // total number of nodes
    
    /* Initialize MPI */
    MPI_Init(&argc, &argv);
    
    /* Identify rank and size for MPI */
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (argc < 3) {
            if (!rank) {
                printf("\nError: Invalid arguments. Please run the program as follows: ./program num_of_frames filename.raw framerate(optional)\n");
            }
            exit(0);
    }
    
    frames = atoi(argv[1]);
    
    /* Rank 0 will control the input/output files and collect execution/delay times */
    if (!rank) {
        printf("\nReading input arguments...\n");
        
        printf("\nFrame count = %d\n", frames);

        strcpy(original_filename, argv[2]);
        
        printf("\nVideo filename = %s\n", original_filename);

        
        if (argc == 4) {
            frame_freq = (float) 1.0/atof(argv[3]); //optionally user can provide his own framerate
        }
        else {
            frame_freq = (float) 1.0/30.0; //default frame rate is 30 frames/sec
        }   
        
        printf("\nFrame frequency = %f sec\n", frame_freq);    
        

        
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
        position = ftell(original_f);
        //printf("\nOffset of original_f pointer (end) = %ld\n", position);
        rewind(original_f); //set position back to beginning    
        position = ftell(original_f);
        //printf("\nOffset of original_f pointer (start) = %ld \n", position);
      
        /* allocate memory for buffer to hold original video file */
        printf("\nAllocating memory for buffers...\n");
        original_buffer = (uint16_t*)malloc(sizeof(uint16_t)*(filesize));
        if (original_buffer == NULL) {
            printf("Memory could not be allocated for the 16-bit original_buffer\n");
            exit(1);
        }
        encrypted_buffer = (unsigned int*)malloc(sizeof(unsigned int)*size*DIM); // number of frames =  number of nodes
        if (encrypted_buffer == NULL) {
            printf("Memory could not be allocated for the 32-bit encrypted_buffer\n");
            exit(1);
        }
        
        /* read original file into original_buffer */
        printf("\nReading original_f...\n");
        fread(original_buffer, sizeof(uint16_t), filesize, original_f);
        //printf("\nOffset of original_f pointer (after reading) = %p\n", original_f);    
        fclose(original_f);
        
        /* open encrypted file */
        printf("\nOpening output file...\n");
        encrypted_f = fopen("encrypted.raw", "w");
        if (encrypted_f == NULL) {
            printf("\nError: Pointer to encrypted_f is NULL.\n");
            exit(1);
        }
        
        /* variables for calculating execution time and controlling frame rate */
        printf("\nStarting operations on original_f...\n");
        exec_start = clock(); //start counting execution time
        frame_start = clock();
        frame_end = frame_start;
    
        delay = 0.0;
        
        printf("\nEncrypting...\n");
    }

    /* generate p, q, n, phi, e, and d for RSA encryption and decryption */
    rsa_generate_components(&rsa_components);
    
    uint16_t *original_frame = (uint16_t*)malloc(sizeof(uint16_t)*DIM);
    unsigned int *encrypted_frame = (unsigned int*)malloc(sizeof(unsigned int)*DIM);  
    
    /* cycle through frames in original_buffer and perform operations on individual frames */
	for (int f = 0; f < frames/size; f++) {
        if (!rank) {
            /* Wait until next frame is available based on frame rate (frame_freq)*/
            clock_t current_time = clock();
            while ((((float) (current_time - frame_start) / CLOCKS_PER_SEC)) <  size*frame_freq) {
                //printf("\nTime passed:   %0.6lf seconds\n", (float) (current_time - frame_start) / CLOCKS_PER_SEC);
                //printf("."); //Waiting on the next frame...\n");
                current_time = clock();
            };        
            
            frame_start = clock();
            delay = delay + (((float) (frame_end - frame_start) / CLOCKS_PER_SEC));
        }
        
        /* MPI Barrier */
        MPI_Barrier(MPI_COMM_WORLD);
        
        /* MPI Scatter */
        MPI_Scatter(&original_buffer[f*DIM*size], DIM, MPI_UINT16_T, original_frame, DIM, MPI_UINT16_T, 0, MPI_COMM_WORLD); //Check here
        
        /* encrypt a frame */     
        rsa_encrypt_frame(original_frame, &rsa_components, encrypted_frame);
        
        /* MPI Gather */
        MPI_Gather(encrypted_frame, DIM, MPI_UNSIGNED, encrypted_buffer, DIM, MPI_UNSIGNED, 0, MPI_COMM_WORLD); //Check here
        
        /* MPI Barrier */
        MPI_Barrier(MPI_COMM_WORLD);
        
        if (!rank) {
            /* write encrypted frame to output file */
            fwrite(encrypted_buffer, size*DIM, sizeof(unsigned int), encrypted_f);
      
            /* Capture when we got finished processing frame */
            frame_end = clock();
        }
        
    }
    
    if (!rank) {
        exec_end = clock(); //stop counting execution time
        printf("\nExecution Time:   %0.6lf seconds\n", (float) (exec_end - exec_start) / CLOCKS_PER_SEC);
        printf("\nTotal Delay Time:   %0.6lf seconds\n", (float) delay);
    }
    
    free(original_frame);
    free(encrypted_frame);
    
    if (!rank) {
        rsa_free_components(&rsa_components);
        free(original_buffer);
        free(encrypted_buffer);
        fclose(encrypted_f);
    }
    
    /* Finialize MPI */
    MPI_Finalize();
    
    return 0;
}
