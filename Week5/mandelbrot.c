#include <stdio.h>
#include <omp.h>

#define NPOINTS 3200
#define MAXITER 7500

#define SMALL_CHUNK_SIZE 1
#define LARGE_CHUNK_SIZE 20

struct complex {
    double r;
    double i;
};

void compute_serial(double *area, double *error) {
    int numinside = 0, numoutside = 0;
    for (int i = 0; i < NPOINTS; i++)
        for (int j = 0; j < NPOINTS; j++)
        {
            // generate grid of points C in the rectangle
            // C.r in [-2  .. 0.5]
            // C.i in [0 .. 1.125 ] - will be taken also symmetric Ox
            struct complex c;
            c.r = -2.0 + 2.5 * (double)(i) / (double)(NPOINTS);
            c.i = 1.125 * (double)(j) / (double)(NPOINTS);
            struct complex z;
            z = c; // start computing series z for c
            for (int iter = 0; iter < MAXITER; iter++)
            {
                double temp = (z.r * z.r) - (z.i * z.i) + c.r;
                z.i = z.r * z.i * 2 + c.i;
                z.r = temp;
                if ((z.r * z.r + z.i * z.i) > 4.0)
                { // z diverges
                    numoutside++;
                    break;
                }
            }
        }
    numinside = NPOINTS * NPOINTS - numoutside;
    *area = 2.0 * 2.5 * 1.125 * (double)(numinside) / (double)(NPOINTS * NPOINTS);
    *error = *area / (double)NPOINTS;
}

void compute_parallel(double *area, double *error) {
    int numinside = 0, numoutside = 0;
#pragma omp parallel for default(shared) schedule(runtime) reduction(+ : numoutside)
    for (int i = 0; i < NPOINTS; i++)
        for (int j = 0; j < NPOINTS; j++)
        {
            // generate grid of points C in the rectangle
            // C.r in [-2  .. 0.5]
            // C.i in [0 .. 1.125 ] - will be taken also symmetric Ox
            struct complex c;
            c.r = -2.0 + 2.5 * (double)(i) / (double)(NPOINTS);
            c.i = 1.125 * (double)(j) / (double)(NPOINTS);
            struct complex z;
            z = c; // start computing series z for c
            for (int iter = 0; iter < MAXITER; iter++)
            {
                double temp = (z.r * z.r) - (z.i * z.i) + c.r;
                z.i = z.r * z.i * 2 + c.i;
                z.r = temp;
                if ((z.r * z.r + z.i * z.i) > 4.0)
                { // z diverges
                    numoutside++;
                    break;
                }
            }
        }
    numinside = NPOINTS * NPOINTS - numoutside;
    *area = 2.0 * 2.5 * 1.125 * (double)(numinside) / (double)(NPOINTS * NPOINTS);
    *error = *area / (double)NPOINTS;
}

void printTimeForFunction(void (*computeFunction)(double *area, double *error), double *area, double *error, char *versionName) {
    double start = 0, time = 0;

    // printf("%s version:...", versionName);
    start = omp_get_wtime();
    computeFunction(area, error);
    time = omp_get_wtime() - start;
    printf("%s:\tarea=%f\terror=%f\ttime=%f\n", versionName, *area, *error, time);
}

int main() {
    double area, error;

    printTimeForFunction(compute_serial, &area, &error, "Serial");

    /*
        schimb schedula dinamic la runtime folosind schedule(runtime) in pragma
        apoi trebuie setata schedula prin omp_set_schedule() cu parametri necesari inainte de fiecare apel de functie
    */

    omp_set_schedule(omp_sched_static, SMALL_CHUNK_SIZE);
    printTimeForFunction(compute_parallel, &area, &error, "Parallel static with chunksize = 1");
    
    omp_set_schedule(omp_sched_static, LARGE_CHUNK_SIZE);
    printTimeForFunction(compute_parallel, &area, &error, "Parallel static with chunksize = 20");

    omp_set_schedule(omp_sched_dynamic, SMALL_CHUNK_SIZE);
    printTimeForFunction(compute_parallel, &area, &error, "Parallel dynamic with chunksize = 1");

    omp_set_schedule(omp_sched_dynamic, LARGE_CHUNK_SIZE);
    printTimeForFunction(compute_parallel, &area, &error, "Parallel dynamic with chunksize = 20");
}