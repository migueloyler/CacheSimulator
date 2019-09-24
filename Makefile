#
# Makefile for Make Some Cache
# 
CC = gcc
CFLAGS = -g -Wall -std=c99

cachesim: cachesim.c cache.c cache.h
	$(CC) $(CFLAGS) -o cachesim cachesim.c cache.c -lm 

#
# Clean the src dirctory
#
clean:
	rm -rf *.o
	rm -f cachesim
	rm -f trace.all trace.f*
	rm -f .cachesim_results .marker
