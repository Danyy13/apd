#include <stdio.h>
#include <omp.h>

int main() {
    // printf("Hello World!\n");
    int myRank = 0;
    int totalThreads = 0;

    #pragma omp parallel sections private(myRank, totalThreads)
    {
        printf("\n");
        myRank = omp_get_thread_num();
        totalThreads = omp_get_num_threads();
#pragma omp section
        {
            printf("Thread %d of %d\n", myRank, totalThreads);
        }
#pragma omp section
        {   
            printf("AAAAAAAAAA\n"); 
        }
    }

    return 0;
}