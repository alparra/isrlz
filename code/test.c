#include "interpolation.h"
#include "suffix_tree.h"
#include "rlz.h"
#include "load.h"
#include <time.h>
#include <stdlib.h>
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

double * test(char * filename, bool bins, Node * suffix_tree){
	srand(time(0)); 
	double *times = malloc(4*sizeof(double));

	char * myfile1 = "../../Data/SCRef/all_data.fsa";
	char * reference = load_file(myfile1, 1);
	//char * myfile2 = "..\\..\\Data\\BY4741_Toronto\\chr16_woheader.fsa";
	char * source = load_file(filename, 0);
	int source_len = strlen(source);
	clock_t t;
	t = clock();
	//Node * suffix_tree = buildSuffixTree(reference);
	printf("Suffix tree built. Proceding to compress...\n");
	if (bins) {
		csb * compressed_bins = compress_bins(suffix_tree, reference, source, 10);
		double delta = get_delta(compressed_bins->lens,compressed_bins->size);
		int largest = largest_bin(compressed_bins->lens);
		printf("Original Length: %d\nNumber of Phrases: %d\nDelta: %.2f\nLargest Bin: %d\n",strlen(source),compressed_bins->size,delta,largest);
		t = clock() - t;
		times[0] = ((double)t) / CLOCKS_PER_SEC; // in seconds 
		t = clock();
		char * y = decompress_bins(reference, compressed_bins);
		t = clock() - t;
		times[1] = ((double)t) / CLOCKS_PER_SEC; // in seconds 


		// Loop for measuring the query range time (10 letters)
		int i, j, ind, rangelen;
		rangelen = 10; 
		struct timespec vartime = timer_start();
		for (i = 0; i < 1000; ++i) {
			ind = rand() % source_len;

			for (j = 0; j < 5; ++j) {
				access_bins_range(reference, compressed_bins, ind, rangelen);
			};
		}
		long time_elapsed_nanos = timer_end(vartime);
		times[3] = time_elapsed_nanos / 1000.0 / rangelen / 5.0;

		// Loop for measuring the query time 
		vartime = timer_start();
		for (i = 0; i < 1000; ++i) {
			ind = rand() % source_len;

			for (j = 0; j < 5; ++j) {
				access_bins(reference, compressed_bins, ind);
			};
		}
		time_elapsed_nanos = timer_end(vartime);
		times[2] = time_elapsed_nanos / 1000.0 / 5.0;

		return times;
	}
	else {
		cs * compressed = compress(suffix_tree, reference, source);
		printf("Original Length: %d\nNumber of Phrases: %d\n",strlen(source),compressed->size);
		t = clock() - t;
		times[0] = ((double)t) / CLOCKS_PER_SEC; // in seconds 
		t = clock();
		char * y = decompress(reference, compressed);
		t = clock() - t;
		times[1] = ((double)t) / CLOCKS_PER_SEC; // in seconds 


		// Loop for measuring the query range time (10 letters)
		int i, j, ind, rangelen;
		rangelen = 10; 
		struct timespec vartime = timer_start();
		for (i = 0; i < 1000; ++i) {
			ind = rand() % source_len;

			for (j = 0; j < 5; ++j) {
				access_range(reference, compressed, ind, rangelen);
			};
		}
		long time_elapsed_nanos = timer_end(vartime);
		times[3] = time_elapsed_nanos / 1000.0 / rangelen / 5.0;

		// Loop for measuring the query time 
		vartime = timer_start();
		for (i = 0; i < 1000; ++i) {
			ind = rand() % source_len;

			for (j = 0; j < 5; ++j) {
				access(reference, compressed, ind);
			};
		}
		time_elapsed_nanos = timer_end(vartime);
		times[2] = time_elapsed_nanos / 1000.0 / 5.0;
	
		return times;
	}
}


/*void test2() {
	char * myfile1 = "..\\..\\Data\\ref3.fsa";
	char * reference = load_file(myfile1);
	Node * suffix_tree = buildSuffixTree(reference);
	int counter = countNodesSuffixTree(suffix_tree, 0); // We have to sum 1 in the end
	printf("Number of nodes: %d \n", counter+1);

}

void test3() {
	char * myfile1 = "..\\..\\Data\\SCRef\\chr16_woheader.fsa";
	char * reference = load_file(myfile1);
	char * myfile2 = "..\\..\\Data\\BY4741_Toronto\\chr16_woheader.fsa";
	char * source = load_file(myfile2);
	clock_t t;
	t = clock();
	Node * suffix_tree = buildSuffixTree(reference);
	printf("Suffix tree built. Proceding to compress...\n");
	cs * compressed = compress(suffix_tree, reference, source);
	csb * compressed_bins = compress_bins(suffix_tree, reference, source);
	int i, ind = 135, len = 20;
	char * res = access_range(reference, compressed, ind, len);
	char * res_bins = access_bins_range(reference, compressed_bins, ind, len);
	printf("No bins: %s\n", res);
	printf("Bins: %s\n", res_bins);
	printf("Original: ");
	for (i = ind; i < ind + len; ++i) {
		printf("%c", source[i]);
	}
	printf("\n");
}


void test4() {
	char * myfile1 = "..\\..\\Data\\SCRef\\chr16_woheader.fsa";
	char * reference = load_file(myfile1);
	char * myfile2 = "..\\..\\Data\\BY4741_Toronto\\chr16_woheader.fsa";
	char * source = load_file(myfile2);
	Node * suffix_tree = buildSuffixTree(reference);
	int counter = countNodesSuffixTree(suffix_tree, 0);
	printf("Suffix tree built. Proceding to compress...\n");
	csb * compressedb = compress_bins(suffix_tree, reference, source);
	cs * compressed = compress(suffix_tree, reference, source);
	printf("Length of source before compression (Kb): %.3f \n", strlen(source) / 1000.0);
	printf("Size of  [cs] compressed source (Kb): %.3f \n", (8 * compressed->size + 4) / 1000.0);
	// CS containts 2 arrays of integers (4 bytes) of length (size) plus the size integer 
	printf("Size of  [csb] compressed source (Kb): %.3f \n", (16 * compressedb->size + 4) / 1000.0);
	// CS containts 4 arrays of integers (4 bytes) of length (size) plus the size integer 
	printf("Size of extra structure to support compression (suffix tree + reference) (Kb): %.3f \n", (strlen(reference) + 32 * (counter + 1)) / 1000.0);
	// Each node in a suffix tree contains 3 pointers (8 bytes) plus 2 integers (4 Bytes) that is 32 bytes per node
}*/

void test5() {
	char * reference = "CATTAGA$";
	char * source = "CATTACATTAGAGACATTAGAGA$";
	clock_t t;
	t = clock();
	Node * suffix_tree = buildSuffixTree(reference);
	printf("Suffix tree built. Proceding to compress...\n");
	cs * compressed = compress(suffix_tree, reference, source);
	csb * compressed_bins = compress_bins(suffix_tree, reference, source, 1);
	int i, ind = 9, len = 10;
	char * res = access_range(reference, compressed, ind, len);
	char * res_bins = access_bins_range(reference, compressed_bins, ind, len);
	//printf("No bins: %s\n", res);
	printf("Bins: %s\n", res_bins);
	printf("Original: ");
	for (i = ind; i < ind + len; ++i) {
		printf("%c", source[i]);
	}
	printf("\n");
}

void test6() {
	char * myfile1 = "../../Data/SCRef/all_data.fsa";
	char * reference = load_file(myfile1, 1);
	Node * suffix_tree = buildSuffixTree(reference);
	int counter = countNodesSuffixTree(suffix_tree, 0);
	// printf("Suffix tree built. Proceding to compress...\n");
	csb ** strains = malloc(36*sizeof(csb));  // We know that we have 36 files in the folder 
	int ind = 0; 
	int totalsize = 0; 
	int totalcsize = 0; 
	// Loop over the source files:
	DIR *dir;
	struct dirent *ent;
	char * name = "../../ProcessedData/"; 
	if ((dir = opendir(name)) != NULL) {
		/* get all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			char filename[100];
			memset(filename, '\0', sizeof(filename));
			strcpy(filename, name); 
			strcat(filename, ent->d_name); 
			printf("%s  \n", filename);
			if (filename[strlen(filename)-1] == '.') {
				printf("Wrong File\n");
				continue;
			}
			double * times = malloc(4 * sizeof(double));
			printf("------------NO BINS------------\n");
			times = test(filename,0,suffix_tree);
			printf("Compression time: %.3f \nDecompression time: %.3f \nQuery time (average) in nsec.: %.3f \nQuery range time 10 chars in nsec.: %.3f\n", times[0], times[1], times[2], times[3]);
			printf("-------------BINS-------------\n");
			times = test(filename,1,suffix_tree);
			printf("Compression time: %.3f \nDecompression time: %.3f \nQuery time (average) in nsec.: %.3f \nQuery range time 10 chars in nsec.: %.3f\n", times[0], times[1], times[2], times[3]);
			//char * source = load_file(filename);
			//totalsize += strlen(source); 
			//csb * compressedb = compress_bins(suffix_tree, reference, source);
			//strains[ind] = compressedb; 
			//totalcsize += 16 * compressedb->size + 4; 
			//printf(" %d  \n ", compressedb->size); 
			//ind += 1; 
		}
		closedir(dir);
	}
	else {
		/* could not open directory */
		perror("");
		return EXIT_FAILURE;
	}


	//printf("Length of set of sources before compression (Mb): %.3f \n", totalsize / 1000000.0);
	//printf("Size of  [csb] compressed set of sources (Mb): %.3f \n", totalcsize / 1000000.0);
	// CS containts 4 arrays of integers (4 bytes) of length (size) plus the size integer 
	// printf("Size of extra structure to support compression (suffix tree + reference) (Mb): %.3f \n", (strlen(reference) + 32 * (counter + 1)) / 1000000.0);
	// Each node in a suffix tree contains 3 pointers (8 bytes) plus 2 integers (4 Bytes) that is 32 bytes per node

}

double * test_(){
	srand(time(0)); 
	double *times = malloc(4*sizeof(double));
	int *arr[8] = { 1,3,6,8,12,13,20,25 };

	struct bins * bins = create_bins(arr, 8, 8);
	int p = 0;
	// printf("Predecessor of %d: %d\n", p, predecessor(bins, p, 8));


	char * myfile1 = "../../Data/SCRef/all_data.fsa";
	char * reference = load_file(myfile1, 1);
	char * myfile2 = "../../Data/all_sources.fsa";
	char * source = load_file(myfile2, 0);
	clock_t t;
	t = clock();
	//Node * suffix_tree = buildSuffixTree(reference);
	//printf("Suffix tree built. Proceding to compress...\n");
	//csb * compressed = compress_bins(suffix_tree, reference, source, 1);
	//csb_to_file(compressed, "./mytrial1.csb"); 
	csb * compressed = file_to_csb("./mytrial1.csb", 1); 
	printf("size of compression (Mb): %.2f \n", (17.0*compressed->size + 8)/1024/1014); 
	t = clock() - t;
	times[0] = ((double)t) / CLOCKS_PER_SEC; // in seconds 
	/* int ind;
	for (ind = 0; ind < strlen(source); ++ind) {
		printf("Access(%d): %c\n", ind, access(reference, compressed, ind));
	}
	*/
	// Print delta of the data
	printf("delta of compressed data: %.4f \n", get_delta(compressed->lens, compressed->size)); 
	printf("largest bin: %d \n", largest_bin(compressed->lens)); 
	int source_len = strlen(source); 
    printf("Number of phrases: %d \n", compressed->size); 
    printf("Number of bins: %d \n", compressed->lens->size); 
	printf("Source length: %d \n", source_len); 
	t = clock();
	char * y = decompress_bins(reference, compressed);
	t = clock() - t;
	times[1] = ((double)t) / CLOCKS_PER_SEC; // in seconds 


	// Loop for measuring the query range time (10 letters)
	int i, j, ind, rangelen;
	rangelen = 8;
	struct timespec vartime = timer_start();
	for (i = 0; i < 2000000; ++i) {
		ind = rand() % (source_len - rangelen);
		for (j = 0; j < 5; ++j) {
			access_bins_range(reference, compressed, ind, rangelen);
		};
	}
	long time_elapsed_nanos = timer_end(vartime);
	times[3] = time_elapsed_nanos / 2000000.0 / rangelen / 5.0; 
	//times[3] = (1000.0*(double)t) / CLOCKS_PER_SEC / 5.0 / rangelen; // in microseconds 

	// Loop for measuring the query time 
	vartime = timer_start();
	for (i = 0; i < 2000000; ++i) {
		ind = rand() % source_len;
		for (j = 0; j < 5; ++j) {
			access_bins(reference, compressed, ind);
		};
	}
	time_elapsed_nanos = timer_end(vartime);
	times[2] = time_elapsed_nanos / 2000000.0 / 5.0; 
	//times[2] = (1000000000000.0*(double)t) / CLOCKS_PER_SEC / 5.0; // in microseconds 

	return times; 
}

