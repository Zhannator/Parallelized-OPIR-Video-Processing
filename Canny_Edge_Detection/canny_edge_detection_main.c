#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

#define IMAGE_WIDTH 256
#define IMAGE_HEIGHT 256

int main(int argc, char **argv)
{
    string file_name;
    if (argc > 1) {
        original_filename = argv[1];
    }
    
    string encrypted_filename = "encrypted.txt";
    
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
    printf("\Offset of original_f pointer (start) = %p\n", original_f);
	fseek(original_f, 0, SEEK_END); //seek the end of file
	long const int filesize = ftell(original_f); //get the size of the file
	int frames = filesize/(IMAGE_WIDTH*IMAGE_HEIGHT*2);
	printf("\nFrame count = %d\n", frames);
	position = ftell(original_f);
	printf("\nOffset of original_f pointer (end) = %ld\n", position);
	rewind(original_f); //set position back to beginning    
    position = ftell(original_f);
	printf("\nOffset of original_f pointer (rewinded) = %ld \n", position);
    
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
	fread(buffer, 2, filesize, original_f);
	printf("\nOffset of original_f pointer (after reading) = %p\n", original_f);    
    
    /* cycle through frames in original_buffer and perform operations on individual frames */
    printf("\nStarting operations on original_f...\n");
	for(int f = 0; f < frames; f++){
        printf("\nFrame number = %d\n", f);
        uint16_t * image_pointer = buffer[0*f*IMAGE_WIDTH*IMAGE_HEIGHT];
        
        /* TODO */
        
    }
   
}