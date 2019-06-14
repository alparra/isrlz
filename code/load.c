/*  
Load module contains all the necessary functions to read source and reference files, 
as well as store the compressed structures in readable text or bytes files.  

Functions: 
load_file
file_to_csb
txt_to_csb
csb_to_txt
csb_to_file
-------------------------------------------------------------------------------------------------
*/


#include <stdio.h>
#include <string.h> 
#include <stdlib.h>

#include "interpolation.h"
#include "suffix_tree.h"
#include "rlz.h"
#include "load.h"

char* load_file(char * filename, int add_N) {
/* This funtions receives as input the file path and returns its content. 
If add_N=1, some N chars are added to the reference. 
This behaviour is meant to be used when the reference file does not contained any undetermined DNA base, 
because suffix_tree cannot handle characters in the source that do not appear in the reference. */

	FILE    *infile;
	char    *buffer;
	long    numbytes;
	char extra_char[30] = "NNNNNNNNNNNNNNNNNNNNNNNNNNNNNN";
	int n_extra = 30;
	infile = fopen(filename, "r");
	if (infile == NULL)
		return 1;

	fseek(infile, 0L, SEEK_END); 	// Get the number of bytes
	numbytes = ftell(infile);
	fseek(infile, 0L, SEEK_SET);
	// grab sufficient memory for the buffer to hold the text
	buffer = (char*)calloc(sizeof(char), numbytes + 1 + 1 + n_extra);

	// memory error
	if (buffer == NULL)
		return 1;
	// copy all the text into the buffer. Add a dollar sign in the end, and N char if needed
	fread(buffer, numbytes, sizeof(char), infile);
	int size = strlen(buffer);
	if (add_N) {
		strcat(buffer, extra_char);
		buffer[size + n_extra] = '$';
		buffer[size + n_extra + 1] = '\0';
	} 
	else {
		buffer[size] = '$';
		buffer[size + 1] = '\0';
	}
	fclose(infile);
	return buffer; 
}

void csb_to_txt(csb * compression, char * filename){
/* This function receives as input a compressed source (csb struct) and writes the compression on the -filename- file. 
The information is written in integers and char text, so it is readable.   */
	FILE * fp;
    int phrase, len; 

	len = compression->size; 
    fp = fopen (filename,"w");
	fprintf(fp, "%d \n", len);
	for(phrase = 0; phrase < len;phrase++){
		fprintf(fp, "%d %d %d \n", compression->starts[phrase], compression->lens->arr[phrase], compression->mismatches[phrase]);
	}; 
	fclose(fp); 
}
void csb_to_file(csb * compression, char * filename){ 
/* This function receives as input a compressed source (csb struct) and writes the compression on the -filename- file. 
The information is written as bytes, so it requires minimum space.   */
	FILE * fp;
	fp = fopen (filename,"w");
	fwrite(&compression->size, sizeof(int), 1, fp);
	fwrite(&compression->lens->size, sizeof(int), 1, fp);
	fwrite(compression->starts, sizeof(int), compression->size, fp);
	
	fwrite(compression->lens->arr, sizeof(int), compression->size, fp);
	fwrite(compression->lens->starts, sizeof(int), compression->lens->size, fp);

	fwrite(compression->mismatches, sizeof(char), compression->size, fp);
	fclose(fp); 
}

csb * txt_to_csb(char * filename, int bin_factor){
/* This function receives as input a -filename- file of a compressed source writen using csb_to_txt function. 
It creates a csb struct with a number of bins depending on the bin_factor specified. 
For ISRLZ implementation, use bin_factor=1.   */
    int phrase, size; 
	FILE* fp = fopen ( filename, "r" );
    fscanf(fp, "%d", &size); 
	csb * compressed_source = malloc(sizeof(csb));
	int *starts = malloc(size * sizeof(int));
	int *lens = malloc(size * sizeof(int));
	char *mismatches = malloc(size * sizeof(char)); 

	for(phrase = 0; phrase < size;phrase++){
		fscanf(fp, "%d %d %d", &starts[phrase], &lens[phrase], &mismatches[phrase]); 
	}; 
	fclose(fp); 
	int num_bins = ceil((double)(phrase + 1) / bin_factor);
	compressed_source->starts = starts;
	compressed_source->lens = create_bins(lens, size, num_bins);
	compressed_source->size = size;
	compressed_source->mismatches = mismatches;
	return compressed_source;
}

csb * file_to_csb(char * filename) {
/* This function receives as input a -filename- file of a compressed source writen using csb_to_file function. 
It returns a csb struct.   */
    int phrase, size, num_bins; 
	FILE* fp = fopen ( filename, "r" );
	fread(&size, sizeof(int), 1, fp); 
	fread(&num_bins, sizeof(int), 1, fp); 

	csb * compressed_source = malloc(sizeof(csb));
	int *starts = malloc(size * sizeof(int));
	int *lens = malloc(size * sizeof(int));

	char *mismatches = malloc(size * sizeof(char)); 
	fread(starts, sizeof(int), size, fp); 

	fread(lens, sizeof(int), size, fp);
	int *bin_starts = malloc(num_bins * sizeof(int));
	fread(bin_starts, sizeof(int), num_bins, fp);

	fread(mismatches, sizeof(char), size, fp);

	compressed_source->size = size; 
	compressed_source->starts = starts;
	struct bins * mybins = malloc(sizeof(struct bins));
	mybins->size = num_bins; 
	compressed_source->lens = mybins; 
	compressed_source->lens->arr = lens;
	compressed_source->lens->starts = bin_starts;
	compressed_source->mismatches = mismatches;
	return compressed_source; 
}

