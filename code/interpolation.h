struct bins
{
	int *starts;
	int *arr;
	int size;
};
int bin_index(int x1, int xn, int xi, int size);
struct bins * create_bins(int *arr, int size, int num_bins);
int predecessor(struct bins * bins, int key, int size);
double get_delta(struct bins * bins, int size);
int largest_bin(struct bins * csbins, int n);
int largest_bin_index(struct bins * bins, int n);
double average_bin(struct bins * bins);
int median_bin(struct bins * bins);
