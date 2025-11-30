/* File:       
 *    mpi_sum.c

Compile with:
gcc -g mpi_sum.c -I "c:\Program Files (x86)\Microsoft SDKs\MPI\Include" -L "c:\Program Files (x86)\Microsoft SDKs\MPI\Lib\x64" -lmsmpi -o mpi_sum.exe
 
*/
#include <stdio.h>
#include "mpi.h"

int main(void) {
    int commSize = 0;
    int myRank = 0;
    int x = 0, y = 0, sum = 0;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &commSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    if(myRank == 0) {
        printf("x = ");
        fflush(stdout);
        scanf("%d", &x);

        printf("y = ");
        fflush(stdout);
        scanf("%d", &y);

        MPI_Send(&x, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Send(&y, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        
        MPI_Recv(&sum, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("Sum: %d\n", sum);
    } else if(myRank == 1) {
        MPI_Recv(&x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&y, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        sum = x + y;

        MPI_Send(&sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

   MPI_Finalize();

    return 0;
}
