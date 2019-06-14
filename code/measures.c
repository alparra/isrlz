/*  
Interpolation module contains all the necessary functions to build and query the bins array (interpolation approach).

'bins' structure refers to the structure that contains the division of a sorted array of integers in different containers.
The definition can be found in interpolation.h header file. 

Functions: 
build_tree_time
compress_time
query_time
query_time_worst
range_query_time
-----------------------------------------------------------------------------------------
*/

#include "interpolation.h"
#include "suffix_tree.h"
#include "rlz.h"
#include "load.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h> 
#include <dirent.h>
#include <stdbool.h>

// call this function to start a nanosecond-resolution timer
struct timespec timer_start(){
    struct timespec start_time;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_time);
    return start_time;
}
// call this function to end a timer, returning nanoseconds elapsed as a long
long timer_end(struct timespec start_time){
    struct timespec end_time;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_time);
    long diffInNanos = (end_time.tv_sec - start_time.tv_sec) * (long)1e9 + (end_time.tv_nsec - start_time.tv_nsec);
    return diffInNanos;
}

double build_tree_time(char * reference) {
/* this function returns the time it takes to function 'buildSuffixTree' from module suffix_tree.c to create a suffix tree from 'reference'.*/ 
	int i;
	clock_t t, t2;
	Node * tree = (Node*)malloc(sizeof(Node));
	t = clock();
	for (i = 0; i < 1; ++i) {
		tree = buildSuffixTree(reference);
		t2 = clock();
		free(tree);
		t += clock() - t2;	
	}
	t = clock() - t;
	return (double)t / CLOCKS_PER_SEC / 1.0;
}

double compress_time(char * filename, char * reference, Node * suffix_tree, int bin_factor) {
/* This function returns the time it takes to function 'compress_bins' from module rlz.c to create an object 'csb' that contains 
the matching of the source in 'filaname' file with respect to the 'reference'. */ 
	char * source = load_file(filename, 0);
	int i;
	csb * compressed_bins = malloc(sizeof(csb));
	clock_t t, t2;
	t = clock();
	for (i = 0; i < 1; ++i) {
		compressed_bins = compress_bins(suffix_tree, reference, source, bin_factor);
		t2 = clock();
		free(compressed_bins);
		t += clock() - t2;
	}
	t = clock() - t;
	return ((double)t) / CLOCKS_PER_SEC / 1.0;
}

double query_time(csb * compressed_bins, char * reference, int num_ind, int source_len) {
/* This function returns the average time it takes to function access_bins from module rlz.c 
to return num_ind random indices of the source compressed in csb structure related to 'reference'.  */ 
	srand(time(0));
	int ind, i, j;
	struct timespec vartime = timer_start();
	for (i = 0; i < num_ind; ++i) {
		ind = rand() % source_len;
	}
	long time_elapsed_nanos = timer_end(vartime);
	struct timespec vartime2 = timer_start();
	for (i = 0; i < num_ind; ++i) {
		ind = rand() % source_len;

		for (j = 0; j < 5; ++j) {
			access_bins(reference, compressed_bins, ind);
		};
	}
	long time_elapsed_nanos2 = timer_end(vartime2);
	return (time_elapsed_nanos2 - time_elapsed_nanos) / 5.0 / num_ind;
}

double query_time_worst(csb * compressed_bins, char * reference, int num_ind) {
/* This function returns the average time it takes to function access_bins from module rlz.c 
to return num_ind random indices. 
The indices are only queried in the fullest bin in the source compressed in csb structure related to 'reference'.  */ 
	srand(time(0));
	int ind, i, j;
	int bin = largest_bin_index(compressed_bins->lens, compressed_bins->size);
	int start = compressed_bins->lens->arr[compressed_bins->lens->starts[bin] + 1];
	int end;
	if (bin < compressed_bins->lens->size - 1)
		end = compressed_bins->lens->arr[compressed_bins->lens->starts[bin + 1] + 1];
	int len = end - start;
	struct timespec vartime = timer_start();
	for (i = 0; i < num_ind; ++i) {
		ind = start + (rand() % len);
	}
	long time_elapsed_nanos = timer_end(vartime);	
	struct timespec vartime2 = timer_start();
	for (i = 0; i < num_ind; ++i) {
		ind = start + (rand() % len);
		for (j = 0; j < 5; ++j) {
			access_bins(reference, compressed_bins, ind);
		};
	}
	long time_elapsed_nanos2 = timer_end(vartime2);
	return (time_elapsed_nanos2 - time_elapsed_nanos) / 5.0 / num_ind;
}

double range_query_time(csb * compressed_bins, char * reference, int range_len, int num_ind, int source_len) {
/* This function returns the average time it takes to function access_bins from module rlz.c 
to return num_ind random ranges of indices of length range_len. 
The string queried is the source compressed in csb structure related to 'reference'.  */ 
	srand(time(0));
	int ind, i, j;
	struct timespec vartime = timer_start();
	for (i = 0; i < num_ind; ++i) {
		ind = rand() % (source_len - range_len);
	}
	long time_elapsed_nanos = timer_end(vartime);
	struct timespec vartime2 = timer_start();
	for (i = 0; i < num_ind; ++i) {
		ind = rand() % (source_len - range_len);

		for (j = 0; j < 5; ++j) {
			access_bins_range(reference, compressed_bins, ind, range_len);
		};
	}
	long time_elapsed_nanos2 = timer_end(vartime2);
	return (time_elapsed_nanos2 - time_elapsed_nanos) / 5.0 / num_ind;
}
