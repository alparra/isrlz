double build_tree_time(char * reference);
double compress_time(char * filename, char * reference, Node * suffix_tree, int bin_factor);
double query_time(csb * compressed_bins, char * reference, int num_ind, int source_len);
double query_time_worst(csb * compressed_bins, char * reference, int num_ind);
double range_query_time(csb * compressed_bins, char * reference, int range_len, int num_ind, int source_len);
