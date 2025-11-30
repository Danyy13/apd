/**
 * file: mpi_ring_fix_blocking.c
 * demonstrates the occurence of deadlock in a ring communication

Compile with:
gcc -g mpi_ring_fix_blocking.c -I "c:\Program Files (x86)\Microsoft SDKs\MPI\Include" -L "c:\Program Files (x86)\Microsoft SDKs\MPI\Lib\x64" -lmsmpi -o mpi_ring_fix_blocking.exe

 **/
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int numtasks, rank, next, prev, a, b, tag = 1;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    prev = rank - 1;
    next = rank + 1;
    if (rank == 0)
        prev = numtasks - 1;
    if (rank == (numtasks - 1))
        next = 0;

    printf("Task %d waits to receive from task %d ... \n", rank, prev);
    fflush(stdout);
    
    // fix
    if(rank == 0) {
        a = rank;
        MPI_Send(&a, 1, MPI_INT, next, tag, MPI_COMM_WORLD);
        printf("Task %d sent %d to task %d\n", rank, a, next);

        MPI_Recv(&b, 1, MPI_INT, prev, tag, MPI_COMM_WORLD, &status);
        printf("Task %d received %d from task %d\n", rank, b, prev);        
    } else {
        MPI_Recv(&b, 1, MPI_INT, prev, tag, MPI_COMM_WORLD, &status);
        printf("Task %d received %d from task %d\n", rank, b, prev);

        a = rank; // put its rank in send buffer
        MPI_Send(&a, 1, MPI_INT, next, tag, MPI_COMM_WORLD);
        printf("Task %d sent %d to task %d\n", rank, a, next);
    }

    MPI_Finalize();

    return 0;
}