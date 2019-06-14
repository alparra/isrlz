#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <string.h>

#include "interpolation.h"
#include "suffix_tree.h"
#include "rlz.h"
#include "load.h"
#include "measures.h"
// ----------------------------------------------------

int main(int argc, char * argv[]) {
	

	if (argc == 1 || (argc == 2 && strcmp(argv[1],"-h") == 0) || (argc == 2 && strcmp(argv[1], "help") == 0)){
		printf("--------------------------------------------------------------------------------------------------------\n");
		printf("--------------------------------------------------------------------------------------------------------\n");
		printf("                                                 ISRLZ HELP \n");
		printf("--------------------------------------------------------------------------------------------------------\n");
		printf("There are four possible actions, determined by the first input: \n'compress', 'decompress', 'access', 'test' \n\n");
		printf("COMPRESS command-line input: \n [reference filename] [source filename] [output filename] (optional)[bin factor] \n\n");
		printf("DECOMPRESS command-line input: \n [reference filename] [compressed source filename] [output filename] \n\n");
		printf("ACCESS command-line input: \n [reference filename] [compressed source filename] [index] (optional)[range length] \n\n");
		printf("TEST command-line input: \n [reference filename] [source filename] [bin factor] [nr of index trials] [nr of range trials] [range length]\n");
		printf("This action will return time measures for access queries (length 1 and length range), compression time, and some statistics of the compression. \n\n");
		printf("for example, you can type:  isrlz compress reference.fsa source_to_compress.fsa compressed_file.csb  \n\n");
		printf("Please notice that [bin factor] command is optional in COMPRESS action. By default, value is 1. \nAlso, [range length] is optional in ACCESS action. By default, only 1 char is returned.  \n");
		printf("--------------------------------------------------------------------------------------------------------\n");
		printf("--------------------------------------------------------------------------------------------------------\n");
		return 1; 
	}
	else if (strcmp(argv[1], "compress") == 0){
		if (argc != 5 && argc != 6){
			printf("leete la ayuda macho \n");
			return 1;
		}
		int bin_factor; 
		char * ref_filename = argv[2];
		char * source_filename = argv[3];
		char * output_filename = argv[4];
		if (argc == 6)
			bin_factor = atoi(argv[5]);
		else  
			bin_factor = 1;
		 
		char * reference = load_file(ref_filename, 1);
		char * source = load_file(source_filename, 0);
		Node * suffix_tree = (Node*)malloc(sizeof(Node));
		suffix_tree = buildSuffixTree(reference);
		csb * compressed_source = compress_bins(suffix_tree, reference, source, bin_factor);
		csb_to_file(compressed_source, output_filename);
		printf("Source string %s has been compressed and stored in file:",source_filename);
		printf(" %s \n", output_filename);  
	}

	else if (strcmp(argv[1], "decompress") == 0){
		if (argc != 5){
			printf("Incorrect command. Please type 'help' or '-h' for a list of the command-line options  \n");
			return 1;
		}	
		char * ref_filename = argv[2];
		char * source_filename = argv[3];
		char * output_filename = argv[4];
		char * reference = load_file(ref_filename, 1);
		csb * compressed_source = file_to_csb(source_filename); 
		char * source = decompress_bins(reference, compressed_source); 
		FILE *fp = fopen(output_filename, "w");
		fputs(source, fp);
		fclose(fp);	
		printf("Source has been decompressed and stored in file:");  
		printf(" %s \n", output_filename);  	
	}
	
	else if (strcmp(argv[1], "access") == 0){
		if (argc != 5 && argc != 6){
			printf("Incorrect command. Please type 'help' or '-h' for a list of the command-line options  \n");
			return 1;
		}
		char * ref_filename = argv[2];
		char * source_filename = argv[3];
		int index = atoi(argv[4]);	
		int len; 
		if (argc == 6)
			len = atoi(argv[5]);
		else 
			len = 0; 
		char * reference = load_file(ref_filename, 1);
		csb * compressed_source = file_to_csb(source_filename); 
		if (len > 0) {
			char * output = access_bins_range(reference, compressed_source, index, len); 
			printf("source[%d..%d] = %s\n", index, index+len, output); 
		}
		else {
			char output = access_bins(reference, compressed_source, index); 
			printf("source[%d] = %c \n", index, output); 	
		}
	}		
	else if (strcmp(argv[1], "test") == 0){
		if (argc != 8){
			printf("Incorrect command. Please type 'help' or '-h' for a list of the command-line options  \n");
			return 1;
		}
		char * ref_filename = argv[2];
		char * source_filename = argv[3];
		int bin_factor = atoi(argv[4]);
		int num_query_ind = atoi(argv[5]);
		int num_range_ind = atoi(argv[6]);
		int range_len = atoi(argv[7]);
		
		char * reference = load_file(ref_filename, 1);
		char * source = load_file(source_filename, 0);
		Node * suffix_tree = (Node*)malloc(sizeof(Node));
		double tree_time, access_time, access_time_worst, range_time;
		printf("Building Suffix Tree...  \n"); 
		suffix_tree = buildSuffixTree(reference);
		tree_time = build_tree_time(reference);
		printf("Suffix Tree construction time: %.3fs\n", tree_time);
		int source_len = strlen(source);
		printf("Compressing...\n");
		csb * compressed_source = compress_bins(suffix_tree, reference, source, bin_factor);
		double comp_time = compress_time(source_filename, reference, suffix_tree, bin_factor);
		printf("Running queries...\n");
		access_time = query_time(compressed_source, reference, num_query_ind, source_len);
		access_time_worst = query_time_worst(compressed_source, reference, num_query_ind);
		range_time = range_query_time(compressed_source, reference, range_len, num_range_ind, source_len);
		double delta = get_delta(compressed_source->lens, compressed_source->size);
		int large_bin = largest_bin(compressed_source->lens, compressed_source->size);
		printf("Results:\n");
		printf("Compression time: %.3fs\n", comp_time);		
		printf("Original length: %d. \nNumber of phrases: %d. \nNumber of bins: %d.\n", source_len, compressed_source->size, compressed_source->lens->size);
		printf("Delta: %.2f. \nLargest bin: %d. \n", delta, large_bin);
		printf("Average time to access %d random indices: %.3fns\n", num_query_ind, access_time);
		printf("Average time to access %d random indices on the fullest bin (worst-case): %.3fns\n", num_query_ind, access_time_worst);
		printf("Average time to access %d random ranges of length %d: %.3fns\n\n", num_range_ind, range_len, range_time);
	}
	else
		printf("Incorrect command. Please type 'isrlz help' or 'isrlz -h' for a list of the command-line options  \n"); 
	return 0;
};
