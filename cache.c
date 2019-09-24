/*
 * cache.c - cache simulator helper functions
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cache.h"
#include <time.h>

/* 
 * printSummary - Summarize the cache simulation statistics. Cache simulators
 *                must call this function in order to be properly autograded. 
 */
void printSummary(int hits, int misses, int evictions) {
    printf("hits:%d misses:%d evictions:%d\n", hits, misses, evictions);
    FILE* output_fp = fopen(".cachesim_results", "w");
    assert(output_fp);
    fprintf(output_fp, "%d %d %d\n", hits, misses, evictions);
    fclose(output_fp);
}

