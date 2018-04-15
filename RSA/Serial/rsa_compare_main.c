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

/* 
    ./program filename1.raw filename2.raw
*/
int main(int argc, char **argv)
{
    char original_filename_1[50];
    char original_filename_2[50];
    
    printf("\nReading input arguments...\n");
    
    if (argc < 3) {
        printf("\nError: Invalid arguments. Please run the program as follows: ./program filename1.raw filename2.raw\n");
        exit(0);
    }
        
    strcpy(original_filename_1, argv[1]);
    strcpy(original_filename_2, argv[2]);
    
    printf("\nVideo filename 1 = %s\n", original_filename_1);
    printf("\nVideo filename 2 = %s\n", original_filename_2);
    
    /* declare file variables */
	FILE *original_f_1;
	FILE *original_f_2;
    
    /* open original files */
	printf("\nOpening original video files...\n");
	original_f_1 = fopen(original_filename_1, "r");
	if (original_f_1 == NULL) {
		printf("\nError: Pointer to original_f_1 is NULL.\n");
        exit(1);
	}
    original_f_2 = fopen(original_filename_2, "r");
    if (original_f_2 == NULL) {
		printf("\nError: Pointer to original_f_2 is NULL.\n");
        exit(1);
	}
    
	/* calculate filesize, number of frames, and identify start and end pointers */
    printf("\nCalculating filesize, number of frames, and identifying start and end pointers...\n");
	long int position_1;
    fseek(original_f_1, 0, SEEK_END); //seek the end of file
	long const int filesize_1 = ftell(original_f_1); //get the size of the file
	int frames_1 = filesize_1/(DIM*2);
	printf("\nFrame count 1 = %d\n", frames_1);
	position_1 = ftell(original_f_1);
	rewind(original_f_1); //set position back to beginning    
    position_1 = ftell(original_f_1);
    long int position_2;
    fseek(original_f_2, 0, SEEK_END); //seek the end of file
	long const int filesize_2 = ftell(original_f_2); //get the size of the file
	int frames_2 = filesize_2/(DIM*2);
	printf("\nFrame count 2 = %d\n", frames_2);
	position_2 = ftell(original_f_2);
	rewind(original_f_2); //set position back to beginning    
    position_2 = ftell(original_f_2);
    
    /* allocate memory for buffer to hold original video file */
    printf("\nAllocating memory for buffers...\n");
    uint16_t *original_buffer_1;
	original_buffer_1 = (uint16_t*)malloc(sizeof(uint16_t)*(filesize_1));
	if (original_buffer_1 == NULL) {
		printf("Memory could not be allocated for the 16-bit original_buffer_1\n");
		exit(1);
	}
    uint16_t *original_buffer_2;
	original_buffer_2 = (uint16_t*)malloc(sizeof(uint16_t)*(filesize_2));
	if (original_buffer_2 == NULL) {
		printf("Memory could not be allocated for the 16-bit original_buffer_2\n");
		exit(1);
	}
    
    if (frames_1 == frames_2) {
        
        /* read original file into original_buffer */
        printf("\nReading original_f...\n");
        fread(original_buffer_1, sizeof(uint16_t), filesize_1, original_f_1);   
        fread(original_buffer_2, sizeof(uint16_t), filesize_2, original_f_2);

        /* cycle through frames in original_buffer and perform operations on individual frames */
        for (int f = 0; f < frames; f++) {
            
            uint16_t *original_frame_1 = &original_buffer_1[f*DIM];
            uint16_t *original_frame_2 = &original_buffer_2[f*DIM];
            
            //Compare original image to decrypted image'
            printf("\nComparing original frame to decrypted frame...\n");
            for(int i = 0; i < DIM; i++){
                if (original_frame_1[i] != original_frame_2[i]) {
                    printf("\noriginal_frame[%d] = %d\n", i, original_frame_1[i]);
                    printf("\ndecrypted_frame[%d] = %d\n", i, original_frame_2[i]);
                    printf("\nDecrypted frame does not match the original frame.\n");
                    exit(1);
                }
            }
            printf("\nOriginal frame and decrypted frame are identical.\n");
        }
    }
    
    free(original_buffer_1);
    free(original_buffer_2);
    fclose(original_f_1);
    fclose(original_f_2);
    
    return 0;
}
