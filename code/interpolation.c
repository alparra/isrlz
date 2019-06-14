/*  
Interpolation module contains all the necessary functions to build and query the bins array (interpolation approach).

'bins' structure refers to the structure that contains the division of a sorted array of integers in different containers.
The definition can be found in interpolation.h header file. 

Functions: 
bin_index
get_delta
largest_bin
largest_bin_index
create_bins

bs_predecessor
predecessor

-----------------------------------------------------------------------------------------
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "interpolation.h"


int bin_index(int x1, int xn, int xi, int size) {
/* this function uses interpolation to return the index of the bin that contains 'xi'. */
	if (x1 == xi)
		return 0;
	return ceil(size*(double)(xi - x1) / (xn - x1)) - 1;
}

double get_delta(struct bins * bins, int size) {
/* this function computes the maximum gap ratio (max_gap/min_gap) of an ordered list of integers (stored on a bins structure). */
	int min = bins->arr[size - 1], max = 0, i;
	for (i = 1; i < size; ++i) {
		int gap = bins->arr[i] - bins->arr[i - 1];
		if (gap < min)
			min = gap;
		if (gap > max)
			max = gap;
	}
	return (double)max / min;
}


int largest_bin(struct bins * csbins, int n){
/* This function returns the number of elements of the fullest bin in the 'csbins' structure */ 
	int max_len = 0, i, new_len; 
	for (i = 0; i < csbins->size - 1; ++i){
		new_len = csbins->starts[i+1] - csbins->starts[i] + 1;
		if (new_len > max_len)
			max_len = new_len; 
	}
	if (n - csbins->starts[csbins->size - 1] > max_len)
		return n - csbins->starts[csbins->size - 1];
	return max_len; 
}

int largest_bin_index(struct bins * bins, int n) {
/* This function returns the index of the fullest bin in the 'bins' structure */ 
	int max_len = 0, i, new_len, ind; 
	for (i = 0; i < bins->size - 1; ++i){
		new_len = bins->starts[i+1] - bins->starts[i] + 1;
		if (new_len > max_len) {
			ind = i;
			max_len = new_len;
		}
	}
	if (n - bins->starts[bins->size - 1] > max_len)
		return bins->size - 1;
	return ind; 
}


struct bins * create_bins(int *arr, int size, int num_bins)
{
/* Given an array of ordered integers, this function creates and returns a bin structure with size/bin_factor bins. 
The structure consists of the starting positions of the bins, the array itself, and the number of bins. */ 
	struct bins * my_bins = malloc(sizeof(struct bins));
	int *starts = malloc(num_bins * sizeof(int));
	int x1 = arr[0];
	int xn = arr[size - 1];
	int i;

	int j = 1; // because the first element is always in the first bin
	int count = 1;
	for (i = 0; i < num_bins; ++i) { 
		while (bin_index(x1,xn,arr[j],num_bins) == i) {
			count += 1;
			j += 1;
		}
		if (i)
		{
			starts[i] = j - count - 1;
		}
		else {
			starts[i] = j - count;
		}
		count = 0;
	}
	my_bins->starts = starts;
	my_bins->arr = arr;
	my_bins->size = num_bins;
	return my_bins;
}

int bs_predecessor(int *arr, int len, int key) {
/* This function performs a predecessor call of number 'key' on the array 'arr' of length 'len' using binary search. */  
	int high = len;
	int low = 0;
	while (high - low > 1) {
		int middle = (high + low) / 2;
		if (key < arr[middle])
			high = middle;
		else if (key > arr[middle])
			low = middle;
		else
			return middle;
	}
	return (key == arr[high]) ? high : low;
}

int predecessor(struct bins * bins, int key, int size){
/* This function returns the predecessor of 'key' in the bin structure 'bins' by 
first performing an interpolation search to find the correct bin and then binary searching with the 'bs_predecessor' function. */  
	int index = bin_index(bins->arr[0], bins->arr[size - 1], key, bins->size);
	if (key < bins->arr[0])
		return 0;
	if (key > bins->arr[size - 1])
		return size - 1;
	return bins->starts[index] + bs_predecessor(&bins->arr[bins->starts[index]], bins->starts[index + 1] - bins->starts[index] + 1, key);
}

