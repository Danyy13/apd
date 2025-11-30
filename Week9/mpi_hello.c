/* File:       
 *    mpi_hello.c

Compile with:
gcc -g mpi_hello.c -I "c:\Program Files (x86)\Microsoft SDKs\MPI\Include" -L "c:\Program Files (x86)\Microsoft SDKs\MPI\Lib\x64" -lmsmpi -o mpi_hello.exe
 
*/
#include <stdio.h>
#include <string.h>  /* For strlen             */
#include "mpi.h"    /* For MPI functions, etc */ 

#define MAX_STRING   100

int main(void) {
   char       greeting[MAX_STRING];  /* String storing message */
   int        comm_sz;               /* Number of processes    */
   int        my_rank;               /* My process rank        */

   /* Start up MPI */
   MPI_Init(NULL, NULL); 

   /* Get the number of processes */
   MPI_Comm_size(MPI_COMM_WORLD, &comm_sz); 

   /* Get my rank among all the processes */
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 

   if (my_rank != 0) { 
      /* Create message */
      sprintf(greeting, "Greetings from process %d of %d!", 
            my_rank, comm_sz);
      /* Send message to process 0 */
      MPI_Send(greeting, strlen(greeting)+1, MPI_CHAR, 0, 0,
            MPI_COMM_WORLD); 
   } else {  
      /* Print my message */
      printf("I am process %d of %d!\n", my_rank, comm_sz);
      for (int q = comm_sz-1; q >0 ; q--) {
         /* Receive message from process q */
         MPI_Recv(greeting, MAX_STRING, MPI_CHAR,MPI_ANY_SOURCE,
            0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
         /* Print message from process q */
         printf("%s\n", greeting);
      } 
   }

   /* Shut down MPI */
   MPI_Finalize();

   return 0;
}  /* main */