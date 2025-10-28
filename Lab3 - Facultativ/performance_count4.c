#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>

/*
Parallel search: count how many times the value x appears in the array a.
The array a is divided in chunks that are given to N_THREADS threads.
Each chunk has elem_per_thread=NELEM/N_THREADS elements.
The thread with number id gets the chunk containing the elements
between index start=id*elem_per_thread and end=(id+1)*elem_per_thread-1.
Each thread searches in its own chunk.

VERSION 4: Each thread works with its own partial counter, lcount[id].value,
which is part of a global variable array. Every thread uses only its own array element.
The array elements are padded such that each array element is on a different cache
line to avoid performance drop because the False Sharing problem!
*/

// #define NELEM 50000000  // number of elements
#define NELEM 10000000  // number of elements
int a[NELEM];
int x = 3;

#define N_THREADS 8
int elem_per_thread = NELEM / N_THREADS;

#define PADDINGSIZE 60

struct padded_int
{
        int value;
        char padding[PADDINGSIZE];  // padding avoids false sharing!
} count[N_THREADS];

// function to become body of thread
// the argument will be thread id
void *count_fct4(void *var)
{
        // thread id
        int id = *(int *)var;
        int start = id * elem_per_thread;         // first elem to be processed
        int end = (id + 1) * elem_per_thread - 1; // last elem to be processes in this thread
        if (id == N_THREADS - 1)
                end = NELEM - 1;

        // printf("thread %d looks between index %d and %d \n", id, start, end);

        for (int i = start; i <= end; i++)
                if (a[i] == x)
                        (count[id].value)++;

        // printf("thread %d has local count %d  \n", id,  count[id]);
        return NULL;
}

int main(int argc, char **argv)
{

        for (int i = 0; i < NELEM; i++)
                a[i] = i % 4;

        int tid[N_THREADS];
        pthread_t thread[N_THREADS];

        int rc;

        struct timespec start, finish;
        double elapsed;

        printf("start\n");
        clock_gettime(CLOCK_MONOTONIC, &start); // measure wall clock time!

        for (int i = 0; i < N_THREADS; i++)
        {
                tid[i] = i;

                rc = pthread_create(&thread[i], NULL, count_fct4, (void *)&tid[i]);
                if (rc)
                {
                        printf("ERROR; pthread_create() return code is %d\n", rc);
                        exit(-1);
                }
        }

        for (int i = 0; i < N_THREADS; i++)
        {
                // the main thread waits for thread1 to finish
                rc = pthread_join(thread[i], NULL);
                if (rc)
                {
                        printf("ERROR; pthread_join() return code is %d\n", rc);
                        exit(-1);
                }
        }

        clock_gettime(CLOCK_MONOTONIC, &finish);

        elapsed = (finish.tv_sec - start.tv_sec);
        elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

        printf("time =%f \n", elapsed);

      /*  for (int i = 0; i < N_THREADS; i++)
                printf("count%d=%d \n", i, count[i].value);
*/
        return 0;
}