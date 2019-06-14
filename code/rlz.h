struct CompressedString {
	int * starts;
	int * lens; // cumulative
	int size;
	char * mismatches; // mismatch stuff
};

struct CompressedStringBins {
	int * starts;
	struct bins * lens; // cumulative
	int size;
	char * mismatches; // mismatch stuff
};

typedef struct CompressedString cs;
typedef struct CompressedStringBins csb;

char find_substring(Node* ref_st, char * reference, char * source, int * tuple);
cs * compress(Node* ref_st, char * reference, char * source);
csb * compress_bins(Node* ref_st, char * reference, char * source, int bin_factor);
char access(char * reference, cs * comp_source, int index);
char access_bins(char * reference, csb * comp_source, int index);
char * access_bins_range(char * reference, csb * comp_source, int i, int len);
char * decompress(char * reference, cs * compressed_source);
char * decompress_bins(char * reference, csb * compressed_source);
