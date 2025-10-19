// Measures speedup for parallelizing the operation between two vectors where
// res[i] = a[i]*a[i] + b[i]*b[i]

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define PRINT_SHORT 0

/* If DEBUG defined, prints arrays*/
// #define DEBUG

/* maximum range for element values */
#define MAXRANGE 5000

// cel mai mare numar de elemente din vector
// #define MAX_SIZE 10000000
// unsigned int numbers[] = {1000000, 2500000, 5000000, 7500000, MAX_SIZE, 0};

#define THREAD_COUNT 8 // numarul fizic de thread-uri de pe Acer = 8

void Usage(char *prog_name);
void Generate_vector(char *prompt, double x[], int n);
void Print_vector(char *title, double y[], double m);
int Equal_vectors(double y[], double z[], double m);

void computeSerial(double a[], double b[], double res[], int n);

void computeParallel(double a[], double b[], double res[], int n);

double showFunctionTime(void (*computation)(double a[], double b[], double result[], int n), double a[], double b[], double result[], int n);
double getElapsedTime(struct timespec start, struct timespec finish);

int n;
double *a = NULL;
double *b = NULL;
double *res = NULL;
double *res_serial = NULL;
// int currentSize = 1000000;

int main(int argc, char *argv[]) {
    printf("Enter n\n"); // Marime vector
    scanf("%d", &n);

    if (n % THREAD_COUNT != 0) {
        printf("n is not divisible by THREAD_COUNT!\n");
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

    printf("Serial: ");
    // computeSerial(a, b, res_serial, n);
    double elapsedSerial = showFunctionTime(computeSerial, a, b, res_serial, n);
#ifdef DEBUG
    Print_vector("Serial result is", res_serial, n);
#endif

    printf("Parallel: ");
    // computeParallel(a, b, res, n);
    double elapsedParallel = showFunctionTime(computeParallel, a, b, res, n);
    #ifdef DEBUG
    Print_vector("Parallel result is", res, n);
#endif

    if (!Equal_vectors(res, res_serial, n)) {
        printf("Error! Serial and Parallel result vectors not equal! \n");
    }

    printf("Measured Speedup=%f\n ", elapsedSerial / elapsedParallel);
    printf("Number of parallel threads was %d\n", THREAD_COUNT);

    free(a);
    free(b);
    free(res);
    free(res_serial);

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
   fprintf(stderr, "usage: %s <THREAD_COUNT>\n", prog_name);
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

int Equal_vectors(double y[], double z[], double m)
{
   int i;
   for (i = 0; i < m; i++)
      if (y[i] != z[i])
         return 0;
   return 1;
}

void computeSerial(double a[], double b[], double res[], int n) {
    for(int i=0;i<n;i++) {
        res[i] = a[i]*a[i] + b[i]*b[i];
    }
}

void *threadFunctionParallel(void *rank) {
    int myRank = *(int *)rank;
    int localN = n / THREAD_COUNT;
    int myStart = myRank * localN;
    int myEnd = (myRank + 1) * localN - 1;

    for(int i=myStart;i<=myEnd;i++) {
        res[i] = a[i]*a[i] + b[i]*b[i];
    }

    return NULL;
}

void computeParallel(double a[], double b[], double res[], int n) {
    int thread;
    pthread_t *thread_handles = malloc(THREAD_COUNT * sizeof(pthread_t));
    int *tid = malloc(THREAD_COUNT * sizeof(int));

    for(thread = 0; thread < THREAD_COUNT; thread++) {
        tid[thread] = thread;
        pthread_create(&thread_handles[thread], NULL, threadFunctionParallel, &tid[thread]);
    }

    for (thread = 0; thread < THREAD_COUNT; thread++) {
        pthread_join(thread_handles[thread], NULL);
    }

    free(thread_handles);
    free(tid);
}

double getElapsedTime(struct timespec start, struct timespec finish) {
    double secondsElapsed = finish.tv_sec - start.tv_sec;
    double nanosecondsElapsed = (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    return secondsElapsed + nanosecondsElapsed;
}

double showFunctionTime(void (*computation)(double a[], double b[], double result[], int n),
    double a[], double b[], double result[], int n
) {
    struct timespec start, finish;
    double elapsedTime = 0;

    clock_gettime(CLOCK_MONOTONIC, &start);

    computation(a, b, result, n);

    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsedTime = getElapsedTime(start, finish);

    if(PRINT_SHORT) {
        printf("%d %.10lf\n", n, elapsedTime);
    } else {
        printf("Wall Clock time for n=%d is %.10lf\n", n, elapsedTime);
    }

    return elapsedTime;
}