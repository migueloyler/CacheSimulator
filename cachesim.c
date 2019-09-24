//Authors: Vincent Dong, Miguel Oyler
//version1


#include "cache.h"
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

#define BUF 1024

typedef unsigned long long int memAddr;

/* struct for cache parameters */
//typedef struct cacheP {
//    int s;
//    int E;
//    int b;
//};

/* struct for a cache line */
typedef struct {
    int valid;
    memAddr tag;
    int LRU;
} cacheL;

/* struct for a cache set, an array of cache lines */
typedef struct {
    cacheL *lines;
} cacheS;

/* struct for a cache, an array of sets */
typedef struct {
    cacheS *sets;
} cache ;

int main(int argc, char** argv) {
    //cacheP flag;
    //variables storing command line input
    int flag_s;
    int flag_E;
    int flag_b;
    char *tracefile;
    char c;

    while ((c = getopt(argc, argv, "hvs:E:b:t:" ) ) != -1 ) { //set all flags
        switch (c) {
        case 'h':
            //printUsageMsg(argv);
            exit(0);
        case 'v':
            //optional
        case 's':
            flag_s = atoi(optarg);
            break;
        case 'E':
            flag_E = atoi(optarg);
            break;
        case 'b':
            flag_b = atoi(optarg);
	    break;
        case 't':
            tracefile = optarg;
	    break;
        default:
            printf("unexpected flag");
            exit(1);
        }
    }

    if (flag_s == 0 || flag_E == 0 || flag_b == 0 || tracefile == NULL) {
        printf("Missing arguement"); //check to see if there are missing arguments
        exit(1);
    }

    cache myCache;
    /* allocating space for the entire cache */
    myCache.sets = malloc((1 << flag_s) * sizeof(cacheS));
    for (int i = 0; i < (1 << flag_s); i++) {
        myCache.sets[i].lines = malloc(flag_E * sizeof(cacheL));
    }

    int totalHit = 0;        //hit counter
    int totalMiss = 0;       //miss counter
    int totalEvict = 0;      //evict counter

    char operation;     //L, S, or M
    memAddr address;
    int size;
    //int mask = 0xFF;
    //int hitF;           //hit flag
    //int evictF;         //evict flag
    //int range;
    //int randomNum;

    FILE * trace_fp = fopen(tracefile, "r");
    char buffer[BUF]; //here is where we read the file line by line

    if (trace_fp == NULL) {
	perror("file not found");
	return 1;
    }
    while (fgets(buffer, sizeof(buffer), trace_fp) != NULL){
	sscanf(buffer, " %c %llx %d", &operation, &address, &size);	
	if (operation != 'I') {
            memAddr curTag = address >> (flag_s + flag_b); //extracting tag
            int tagSize = 64 - flag_s - flag_b;
            unsigned long long setNum = (address << tagSize) >> (tagSize + flag_b); //extracting setNumber
	    
            cacheS mySet = myCache.sets[setNum];
	    int hit = 0; //hit flag, in case we get a hit
	    int isFull = 1; //check if our cache is full, default to 1, we assume its full
	    int emptyLineIndex; 
	    int minLRU = 420; //to determine which line was LRU
	    int maxLRU = 0; //to determine which line was MRU
	    int indexOfMin = 0; //to find which line was LRU
	    int curLRU = 420; 
            for (int j = 0; j < flag_E; j++) { //loop through lines
		if (mySet.lines[j].valid != 1) { //check if there are invalid lines, if there are
		    isFull = 0; //then it means our cache isn't full
		    emptyLineIndex = j;
		    continue;
		}
		if (mySet.lines[j].tag == curTag) { //check if tag matches
		    hit = 1; //we got a hit
		    mySet.lines[j].LRU++;
		    break; //we break so we don't do unnecessary work
		}
	    }
	    if (hit != 0) {
		totalHit++; //we got a hit
	    } else { //we didn't get a hit by the end of the loop, therefore we missed
		totalMiss++;
		if (isFull == 0) { //check if we need to evict
		    mySet.lines[emptyLineIndex].tag = curTag; //update the cache
		    mySet.lines[emptyLineIndex].valid = 1; //this block is now valid
		    mySet.lines[emptyLineIndex].LRU++;
		} else { //our cache is full, we need to evict
		    for(int j = 0; j < flag_E; j++) { //loop through to find the LRU block
			curLRU = mySet.lines[j].LRU;
			if (curLRU <= minLRU) {
			    minLRU = curLRU; //update the min
			    indexOfMin = j; //get the index because we'll need it to update the cache upon evict
			}
			if (curLRU >= maxLRU){
			    maxLRU = curLRU;
			}
		    }
		    totalEvict++;
		    mySet.lines[indexOfMin].tag = curTag; //evict / update cache
		    mySet.lines[indexOfMin].LRU = maxLRU; 
		}

	    }

	    
	    if (operation == 'M') {
		totalHit++;
	    }
	    

	    
	}
    }
    printSummary(totalHit, totalMiss, totalEvict);
    return 0;
}
