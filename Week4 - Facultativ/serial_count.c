#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>

/*
Serial search: count how many times the value x appears in the array a.
VERSION 0: No threads used, just plain sequential search
*/

// #define NELEM 50000000  // number of elements
#define NELEM 10000000  // number of elements
int a[NELEM];
int x = 3;   // value x to be searched for in array a
int count;   // how many times appears x in array a?

int main(int argc, char *argv[]) {
    // initialize values of array a
	// a: 0 1 2 3 0 1 2 3 0 1 2 3 ...
	for (int i = 0; i < NELEM; i++)
		a[i] = i % 4;

    printf("start\n");

	struct timespec start, finish;
	double elapsed;

	clock_gettime(CLOCK_MONOTONIC, &start); // measure wall clock time!

	for(int i=0;i<NELEM;i++) {
        if(a[i] == x) {
            count++;
        }
    }

	clock_gettime(CLOCK_MONOTONIC, &finish);

	elapsed = (finish.tv_sec - start.tv_sec);
	elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

	printf("time =%f \n", elapsed);
	printf("count=%d \n", count);
}