// Measures speedup for parallelizing the operation between two vectors where
// res[i] = a[i]*a[i] + b[i]*b[i]

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/* If DEBUG defined, prints arrays*/
#define DEBUG

/* maximum range for element values */
#define MAXRANGE 5000

void Usage(char *prog_name);
void Generate_vector(char *prompt, double x[], int n);
void Print_vector(char *title, double y[], double m);

void computeSerial(double a[], double b[], double res[], int n);

void computeParallel(double a[], double b[], double res[], int n);

int thread_count;
int n;
double *a = NULL;
double *b = NULL;
double *res = NULL;
double *res_serial = NULL;

int main(int argc, char *argv[]) {
    // Generate vectors
    if (argc != 2) {
        Usage(argv[0]);
    }
    thread_count = atoi(argv[1]);


    printf("Enter n\n"); // Marime vector
    scanf("%d", &n);

    if (n % thread_count != 0) {
        printf("n is not divisible by thread_count!\n");
        exit(1);
    }

    a = malloc(n * sizeof(double));
    b = malloc(n * sizeof(double));
    res = malloc(n * sizeof(double));
    res_serial = malloc(n * sizeof(double));

    Generate_vector("Generate the vector", a, n);
#ifdef DEBUG
    Print_vector("Vector is", a, n);
#endif

    Generate_vector("Generate the vector", b, n);
#ifdef DEBUG
    Print_vector("Vector is", b, n);
#endif

    computeSerial(a, b, res_serial, n);
#ifdef DEBUG
    Print_vector("Serial result is", res_serial, n);
#endif

    computeParallel(a, b, res, n);
#ifdef DEBUG
    Print_vector("Parallel result is", res, n);
#endif

    return 0;
}

/*------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   print a message showing what the command line should
 *            be, and terminate
 * In arg :   prog_name
 */
void Usage(char *prog_name)
{
   fprintf(stderr, "usage: %s <thread_count>\n", prog_name);
   exit(0);
} /* Usage */

/*------------------------------------------------------------------
 * Function:        Generate_vector
 * Purpose:         Generate random the vector x
 * In arg:          prompt, n
 * Out arg:         x
 */
void Generate_vector(char *prompt, double x[], int n)
{
   int i;

   printf("%s\n", prompt);
   for (i = 0; i < n; i++)
      // scanf("%lf", &x[i]);
      x[i] = rand() % MAXRANGE;
} /* Generate_vector */

/*------------------------------------------------------------------
 * Function:    Print_vector
 * Purpose:     Print a vector
 * In args:     title, y, m
 */
void Print_vector(char *title, double y[], double m)
{
   int i;

   printf("%s\n", title);
   for (i = 0; i < m; i++)
      printf("%4.1f ", y[i]);
   printf("\n");
} /* Print_vector */

void computeSerial(double a[], double b[], double res[], int n) {
    for(int i=0;i<n;i++) {
        res[i] = a[i]*a[i] + b[i]*b[i];
    }
}

void *threadFunctionParallel(void *rank) {
    int myRank = *(int *)rank;
    int localN = n / thread_count;
    int myStart = myRank * localN;
    int myEnd = (myRank + 1) * localN - 1;

    for(int i=myStart;i<=myEnd;i++) {
        res[i] = a[i]*a[i] + b[i]*b[i];
    }

    return NULL;
}

void computeParallel(double a[], double b[], double res[], int n) {
    int thread;
    pthread_t *thread_handles = malloc(thread_count * sizeof(int));
    int *tid = malloc(thread_count * sizeof(int));

    for(thread = 0; thread < thread_count; thread++) {
        tid[thread] = thread;
        pthread_create(&thread_handles[thread], NULL, threadFunctionParallel, &tid[thread]);
    }

    for (thread = 0; thread < thread_count; thread++) {
        pthread_join(thread_handles[thread], NULL);
    }

    free(thread_handles);
    free(tid);
}