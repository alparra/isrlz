char * load_file(char* filename, int add_N);
void csb_to_file(csb * compression, char * filename); 
csb * file_to_csb(char * filename);  
void csb_to_txt(csb * compression, char * filename); 
csb * txt_to_csb(char * filename, int bin_factor);  
