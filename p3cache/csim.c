#include "cachelab.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int valid, tag, setindex, blocksize, address;
    int LRUcounter;
    char memtype;
} Line;

/*
typedef struct {
    Line *NumberOfLinesPerSet;
} Set;*/


//// function to calculate x raised to the power y taken from geeksforgeeks.com /////////

int power(int x, unsigned int y) {
    if (y == 0)
	return 1;
    else if (y%2 == 0)
	return power(x, y/2)*power(x, y/2);
    else
	return x*power(x, y/2)*power(x, y/2);
}

////////////////////////////////////////////////////////////////////////////
// To find the length, taken from http://stackoverflow.com/questions/3068397/c-how-to-find-the-length-of-an-integer
int lenHelper(unsigned x) {
    if(x>=1000000000) return 10;
    if(x>=100000000) return 9;
    if(x>=10000000) return 8;
    if(x>=1000000) return 7;
    if(x>=100000) return 6;
    if(x>=10000) return 5;
    if(x>=1000) return 4;
    if(x>=100) return 3;
    if(x>=10) return 2;
    return 1;
}
/////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
    // Check arg options ////////////////////////////////////////////////////////////
    extern char *optarg;
    extern int optind;
    int c;
    int hflag = 0, vflag = 0, sflag = 0, Eflag = 0, bflag = 0, tflag = 0, errorflag = 0;
    int sbits, Elines, bbits;
    int S, B;
    char *tracefile;
    int i;
    int n = 0;

    while (( c = getopt(argc, argv, "hvs:E:b:t:")) != -1)
	switch (c) {
	    case 'h':
		hflag = 1;
		break;
	    case 'v':
		vflag = 1;
		break;
	    case 's':
		sflag = 1;
		sscanf(optarg, "%d", &n);
		sbits = n;
		break;
	    case 'E':
		Eflag = 1;
		sscanf(optarg, "%d", &n);
		Elines = n;
		break;
	    case 'b':
		bflag = 1;
		sscanf(optarg, "%d", &n);
		bbits = n;
		break;
	    case 't':
		tflag = 1;
		tracefile = optarg;
		break;
	    case '?':
		errorflag = 1;
		break;
	}

    //Check mandatory options and set up cache
    if (Eflag == 0) {
	fprintf(stderr, "%s missing '-E' param: number of lines per set (Associativity).  Add '-h' for program usage.", argv[0]);
    }

    if (hflag == 1) {
	fprintf(stderr, "\nProgram arguments: -s <number of set index bits> \n\t\t   -E <Associativity (Number of lines per set)> \n\t\t   -b <Number of block bits> \n\t\t   -t <tracefile (Name of the valgrind trace to replay)> \n\t\t   -h (optional - displays program usage) \n\t\t   -v (optional - displays result in verbose mode; displays more detailed trace info) \n\nThis program is meant to simulate a computer cache, the parameters specify the size of the cache and requires a valgrind trace as input.  The output displays the number of cache hits, misses, and evictions (In verbose mode, the traces are listed with the corresponding 'hit/miss/eviction' status)  \n\nWritten by Shelby Williams 2014 \n");
    }

    if (bflag == 0) {
	fprintf(stderr, "\n%s missing '-b' param: number of block bits.  Add '-h' for program usage.\n", argv[0]);
    }

    if (tflag == 0) {
	fprintf(stderr, "\n%s missing '-t' param: name of valgrind trace file.  Add '-h' for program usage.\n", argv[0]);
    }

    if (errorflag == 1) {
	fprintf(stderr, "\nOption/parameter not recognized in %s, try again.  Add '-h' for program usage.\n", argv[0]);
	return 0;
    }

    if (sflag == 0) {
	fprintf(stderr, "\n%s missing '-s' param: number of set index bits. Add '-h' for program usage.\n", argv[0]);
    }
   
        S = power(2, sbits);
        Line Cache[S*Elines];
	for (i = 0; i < S*Elines; i++) {
	    Cache[i].valid = 0;
	    Cache[i].LRUcounter = 0;
	}
	
    fprintf(stderr, "\nS = %d, sbits = %d, Elines = %d, bbits = %d, tracefile = %s\n", S, sbits, Elines, bbits, tracefile);

    //// Reading the tracefile /////////////////////////////////////////////////////////////////////////////////////////////////

	FILE * fp;
	size_t length = 0;
	ssize_t read;
	char * tempLine = NULL;
	fp = fopen(tracefile, "r");
	if (fp == NULL) {
	    fprintf(stderr, "Unable to open trace file.");
	    return 0;
	}
/// get line count ///
    int linecount = 0;
    while ((read = getline(&tempLine, &length, fp)) != -1) {
        linecount++;
    }

    fclose(fp);

/// save trace lines into array of strings ///
    fp = fopen(tracefile, "r");

    char *traceArray[linecount];
    char mode;
    unsigned long long addr;
    //int trash, trash2;
    int j;
    int tagsize = 64 - (sbits + bbits);
    
    int miss = 0;
    int Hits = 0;
    int eviction = 0;
    
    while (j < linecount) {
        getline(&tempLine, &length, fp);
        //RemoveSpaces(tempLine);
        traceArray[j] = malloc(strlen(tempLine)+1);
        sscanf(tempLine, " %c %11x,%*d", &mode, &addr);
        unsigned long long atag = addr >> (sbits + bbits);
	unsigned long long setI = ((addr << tagsize) >> (tagsize + bbits));
        
        
        int Lflag = 0, Sflag = 0, Mflag = 0;
        
        if (mode == 'L') {
		fprintf(stderr, "L was used");
		Lflag = 1;	
	}
	else if (mode == 'S') {
		fprintf(stderr, "S was used");
		Sflag = 1;
	}
	else if (mode == 'M') {
		fprintf(stderr, "M was used");
		Mflag = 1;
	}
	else {
		fprintf(stderr, "%c %llx ", mode, addr);
	}
	
	int count = 1;
	int Hflag = 0;
	for (i = 0; i < S * Elines; i++) {
		Hflag = 0;
		if (Cache[i].tag == atag) {
			++Hits;
			Cache[i].LRUcounter = count;
			++count;
			int Hflag = 1;
		}
		}
	if (Hflag == 0) { 
		++miss;
		for (i = 0; i < S*Elines; i++) {
		if (Cache[i].LRUcounter == 0) {
				Cache[i].tag = atag;
				Cache[i].LRUcounter = count;
				Cache[i].valid = 1;
				break;
				}
				
		else if (Cache[i].LRUcounter == count) {}
		else if (Cache[i].LRUcounter > count) {}
		else {
			++eviction;
			Cache[i].tag = atag;
			Cache[i].LRUcounter = count;
			break;
			}
			}
			}
	
	
        j++;
	
    } 

    //fprintf(stderr, "\nS = %d, sbits = %d, Elines = %d, B = %d, bbits = %d, tracefile = %s, misses = %d, hits = %d, evictions = %d\n", S, sbits, Elines, B, bbits, tracefile, miss, Hits, eviction);

    printSummary(Hits, miss, eviction); 
    return 0;
}
