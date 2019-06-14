/*  
Rlz module contains all the necessary functions to compress, access and decompress a source file
given a reference file, using the LZ-77 schema.  
csb (CompressedStringBins) structure refers to the compressed source using the bins array (interpolation approach), 
while cs (CompressedString) structure refers to the naive compressed source using just LZ-77. 
Both definitions can be found in rlz.h header file. 

Functions: 
find_substring
compress_bins
access_bins
decompress_bins
compress
access
decompress
-----------------------------------------------------------------------------------------
*/


#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 

#include "interpolation.h"
#include "suffix_tree.h"
#include "rlz.h"

// Look-up table to codify ASCII chars into positions of a small array (trick for the suffix_tree)
short lookup2[256] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 3, 0, 4, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 6, 0,
0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

char find_substring(Node* ref_st, char * reference, char * source, int * tuple) {
/*  This function finds the longest common prefix between the reference and the source. 
In order to do it, it walks through the suffix_tree of the reference string (ref_st node). 
The compression (ref_index, length) is stored on the tuple parameter. 
The function returns the fist mismatch character of the source, to be used on the compression.  */
	tuple[1] = 0;
	char curr_char = source[0];
	Node * curr_node = ref_st;
	int curr_len = 0;
	while (curr_node->children[lookup2[curr_char] - 1] != NULL) {
		curr_node = curr_node->children[lookup2[curr_char] - 1];
		int j;
		for (j = 0; j < *curr_node->end - curr_node->start + 1; ++j) {
			if (reference[curr_node->start + j] != source[curr_len]) {
				tuple[0] = curr_node->suffixIndex;
				tuple[1] = curr_len + 1;
				return source[curr_len];
			}
			else {
				++curr_len;
			}
		}
		if (curr_char == "$")
		{
			tuple[0] = curr_node->suffixIndex;
			tuple[1] = curr_len;
			return '$';
		}
		curr_char = source[curr_len];
	}
	tuple[0] = curr_node->suffixIndex;
	tuple[1] = curr_len + 1;
	return source[curr_len];
}


cs * compress(Node* ref_st, char * reference, char * source) {
/*  This function finds the compression of source relative to reference. 
In order to do it, it calls the find_substring function and stores the subsequently results on 
the 3 arrays containing starts, lengths and mismatches.   */
	int i = 0;
	int source_len = strlen(source);
	cs * compressed_source = malloc(sizeof(cs));
	int *starts = malloc(source_len * sizeof(int));
	int *lens = malloc(source_len * sizeof(int));
	char *mismatches = malloc(source_len * sizeof(char));
	int *tuple = malloc(2 * sizeof(int));
	starts[0] = 0;
	lens[0] = 0;
	mismatches[0] = 0;
	tuple[0] = 0;
	tuple[1] = 0;
	int phrase = 0;
	while (i < source_len) {
		phrase += 1;
		mismatches[phrase] = find_substring(ref_st, reference, &source[i], tuple);
		starts[phrase] = tuple[0];
		lens[phrase] = lens[phrase - 1] + tuple[1];
		i = i + tuple[1];
	}
	realloc(*starts, phrase * sizeof(int));
	realloc(*lens, phrase * sizeof(int));
	realloc(*mismatches, (phrase) * sizeof(char));
	compressed_source->starts = starts;
	compressed_source->lens = lens;
	compressed_source->size = phrase + 1;
	compressed_source->mismatches = mismatches;
	return compressed_source;
}


csb * compress_bins(Node* ref_st, char * reference, char * source, int bin_factor) {
/*  This function finds the compression of source relative to reference. 
In order to do it, it calls the find_substring function and stores the subsequently results on 
the 3 arrays containing starts, lengths and mismatches. 
Finally, the lengths are stored on a bins_array with number of bins depending on the bin_factor.
For ISRLZ implementation, use bin_factor=1  */

	int i = 0;
	int source_len = strlen(source);
	csb * compressed_source = malloc(sizeof(csb));
	int *starts = malloc(source_len * sizeof(int));
	int *lens = malloc(source_len * sizeof(int));
	char *mismatches = malloc(source_len * sizeof(char)); 
	int *tuple = malloc(2 * sizeof(int));
	starts[0] = 0;
	lens[0] = 0;
	mismatches[0] = 0; 
	tuple[0] = 0;
	tuple[1] = 0;
	int phrase = 0;
	while (i < source_len) {
		phrase += 1;
		mismatches[phrase] = find_substring(ref_st, reference, &source[i], tuple);
		starts[phrase] = tuple[0];
		lens[phrase] = lens[phrase - 1] + tuple[1];
		i = i + tuple[1];
		if (tuple[1] == 0) {
			printf("Error. Character in source not in ref\n");
			break;
		}
	}
	realloc(*starts, phrase * sizeof(int));
	realloc(*lens, phrase * sizeof(int));
	realloc(*mismatches, (phrase) * sizeof(char));
	compressed_source->starts = starts;
	int num_bins = ceil((double)(phrase + 1) / bin_factor);
	compressed_source->lens = create_bins(lens, phrase + 1, num_bins);
	compressed_source->size = phrase + 1;
	compressed_source->mismatches = mismatches;
	return compressed_source;
}

char access(char * reference, cs * comp_source, int i) {
/* This function returns the character in position i of the original source that is compressed on the comp_source structure. 
It works as a naive implementation using binary search for predecessor queries. */
	int index = bs_predecessor(comp_source->lens, comp_source->size, i);
	int char_index = i - comp_source->lens[index]; 
	return (char_index == comp_source->lens[index + 1] - 1) ? comp_source->mismatches[index + 1] : reference[char_index + comp_source->starts[index + 1]];
	
}


char access_bins(char * reference, csb * comp_source, int i) {
/* This function returns the character in position i of the original source that is compressed on the comp_source structure. 
It is based on interpolation search predecessor. */
	int index = predecessor(comp_source->lens, i, comp_source->size);
	int char_index = i - comp_source->lens->arr[index];
	return (char_index == comp_source->lens->arr[index + 1] - 1) ? comp_source->mismatches[index + 1] : reference[char_index + comp_source->starts[index + 1]];
	// this +1 will never go out because the last element in the cumsum list is the length of the array and the access index will always be lower than the length (at most len - 1)
}

char * access_range(char * reference, cs * comp_source, int i, int len) {
/* This function returns the characters in position [i, i+len] of the original source that is compressed on the comp_source structure. 
It is based on binary search. */
	char * res = malloc(len * sizeof(char) + 1);
	int index = bs_predecessor(comp_source->lens, comp_source->size, i);
	int char_index = i - comp_source->lens[index];
	res[0] = (char_index == comp_source->lens[index + 1] - 1) ? comp_source->mismatches[index + 1] : reference[char_index + comp_source->starts[index + 1]];
	int count = 1;
	char_index += 1;
	while (count < len && ((index - 1) < comp_source->size)) {
		int check = comp_source->lens[index + 1] - comp_source->lens[index] - char_index;
		if (check > 0) {
			res[count] = (check == 1) ? comp_source->mismatches[index + 1] : reference[char_index + comp_source->starts[index + 1]];
			char_index += 1;
			count += 1;
		}
		else {
			index += 1;
			char_index = 0;
		}
	}
	res[len] = '\0';
	return res;
}

char * access_bins_range(char * reference, csb * comp_source, int i, int len) {
/* This function returns the characters in position [i, i+len] of the original source that is compressed on the comp_source structure. 
It is based on interpolation search for predecesor queries. */
	char * res = malloc(len * sizeof(char) + 1);
	int index = predecessor(comp_source->lens, i, comp_source->size);
	int char_index = i - comp_source->lens->arr[index];
	res[0] = (char_index == comp_source->lens->arr[index + 1] - 1) ? comp_source->mismatches[index + 1] : reference[char_index + comp_source->starts[index + 1]];
	int count = 1;
	char_index += 1;
	while (count < len && ((index-1) < comp_source->size)){
		int check = comp_source->lens->arr[index + 1] - comp_source->lens->arr[index] - char_index;
		if (check > 0) {
			res[count] = (check == 1) ? comp_source->mismatches[index + 1] : reference[char_index + comp_source->starts[index + 1]];
			char_index += 1;
			count += 1;
		}
		else {
			index += 1;
			char_index = 0;
		}
	}
	res[len] = '\0';
	return res;
}

char * decompress(char * reference, cs * compressed_source) {
/* This function returns the original string source codified in the compressed_source structure (cs) */
	char * source = calloc((compressed_source->lens[compressed_source->size - 1] + 1), sizeof(char));
	int i, j, cont = 0;
	int * lens = compressed_source->lens;
	int * starts = compressed_source->starts; // we assume this is a pointer and costs no extra memory
	for (i = 1; i < compressed_source->size; ++i) {
		int limit = lens[i] - lens[i - 1];
		for (j = 0; j < limit; ++j) {
			source[cont] = (j == limit - 1) ? compressed_source->mismatches[i] : reference[starts[i] + j];
			cont++;
		}
	}
	return source;
}

char * decompress_bins(char * reference, csb * compressed_source) {
/* This function returns the original string source codified in the compressed_source structure (csb) */
	char * source = calloc((compressed_source->lens->arr[compressed_source->size - 1]+1), sizeof(char));
	int i, j, cont = 0;
	int * lens = compressed_source->lens->arr; 
	int * starts = compressed_source->starts; 
	for (i = 1; i < compressed_source->size; ++i) {
		int limit = lens[i] - lens[i - 1];
		for (j = 0; j < limit; ++j) {
			source[cont] = (j == limit - 1) ? compressed_source->mismatches[i] : reference[starts[i] + j];
			cont++;
		}
	}
	return source; 
}


