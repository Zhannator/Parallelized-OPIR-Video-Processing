#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define IMAGE_WIDTH 256
#define IMAGE_HEIGHT 256
#define DIM 65536 //IMAGE_WIDTH * IMAGE_HEIGHT

int main(int argc, char **argv)
{
    char original_filename[50];
    if (argc > 1) {
        strcpy(original_filename, argv[1]);
    }
    
    char encrypted_filename[50] = "encrypted.txt";
    
    /* declare file variables */
	FILE* original_f;
	FILE* encrypted_f;
    
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
    uint16_t * original_buffer;
	original_buffer = (uint16_t*)malloc(sizeof(uint16_t)*(filesize));
	if (original_buffer == NULL){
		printf("Memory could not be allocated for the 16-bit original_buffer\n");
		exit(1);
	}
    
    /* read original file into original_buffer */
	printf("\nReading original_f...\n");
	fread(original_buffer, 2, filesize, original_f);
	printf("\nOffset of original_f pointer (after reading) = %p\n", original_f);    
    
    /* cycle through frames in original_buffer and perform operations on individual frames */
    printf("\nStarting operations on original_f...\n");
	for(int f = 0; f < frames; f++){
        printf("\nFrame number = %d\n", f);
        uint16_t * frame = &original_buffer[f*DIM];
        
        /* TODO */
        
    }
   
    free(original_buffer);
    
    fclose(original_f);
}