/* File:  abc.c
Creates 3 threads.
First thread repeatedly writes A.
Second thread repeatredly writes B.
Third thread repeatedly writes C.
The main thread repeatedly writes newline.

Add synchronization such that the output is always in the form:
on each new line there is a random permutation of A,B,C.
You can add any synchronization that you want, but you 
can not change the existing printf's or add new printf's


For example, the output can be:
ABC
BAC
ABC
BCA
...
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_barrier_t letterBarrier;

#define REPEAT 100
#define THREAD_COUNT 3

/* Thread function A */
void *HelloA(void *dummy)
{
    for (int i = 0; i < REPEAT; i++)
        printf("A");
        
    pthread_barrier_wait(&letterBarrier);

    return NULL;
}

/* Thread function B */
void *HelloB(void *dummy)
{
    for (int i = 0; i < REPEAT; i++)
        printf("B");

    pthread_barrier_wait(&letterBarrier);

    return NULL;
}

/* Thread function C */
void *HelloC(void *dummy)
{
    for (int i = 0; i < REPEAT; i++)
        printf("C");

    pthread_barrier_wait(&letterBarrier);

    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t thread_handleA, thread_handleB, thread_handleC;
    
    pthread_barrier_init(&letterBarrier, NULL, THREAD_COUNT);

    pthread_create(&thread_handleA, NULL, HelloA, NULL);
    pthread_create(&thread_handleB, NULL, HelloB, NULL);
    pthread_create(&thread_handleC, NULL, HelloC, NULL);

    for (int i = 0; i < REPEAT; i++)
        printf("\n");

    pthread_join(thread_handleA, NULL);
    pthread_join(thread_handleB, NULL);
    pthread_join(thread_handleC, NULL);

    pthread_barrier_destroy(&letterBarrier);

    return 0;
}
