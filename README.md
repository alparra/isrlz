# ISRLZ

_Interpolation Search - Relative Lempel Ziv_ is a C implementation for compressing genomes. It is based on the MSc Thesis project "Relative Compression for Highly Repetitive Data", from the Technical University of Denmark. The repository contains all the necessary code, plus some data samples included in /Data folder.   

## Installation


```bash
git clone https://github.com/alparra/isrlz
cd isrlz/code
make
```

## Usage

Example:  
```bash
isrlz compress ../Data/Drosophila/s5.fsa ../Data/Drosophila/s13.fsa comp_s13_ref_s5.csb 4 
```


The module expects DNA string files (only characters **A, C, T, G, N** are allowed).  
Then, there are four possible actions that can be executed: _compress_, _decompress_, _access_, and _test_. 

In order to compress ```source file``` against ```reference file```, type: 
```bash
isrlz compress [reference filename] [source filename] [output filename] (optional)[bin factor] 
```
Please notice that [bin factor] command is optional in COMPRESS action. By default, value is 1.  

In order to decompress ```compressed source file``` related to ```reference file```, type: 
```bash
isrlz decompress [reference filename] [compressed source filename] [output filename] 
```
In order to access one or more characters of the original ```source file``` by its index, type: 
```bash
isrlz access [reference filename] [compressed source filename] [index] (optional)[range length] 
```
[range length] is optional in ACCESS action. By default, only 1 char is returned.  

 
In order to obtain information about the compression, you can use the action TEST. This action will return time measures for access queries (length 1 and length range), compression time, and some statistics of the compression: 
```bash
islrz test [reference filename] [source filename] [bin factor] [nr of index trials] [nr of range trials] [range length]
```
  




## Authors
Arnau Sanromà Mani  
Alicia Parra Acero
