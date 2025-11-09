/**
 * Heat 2D 
 * A rectangular plane of material has heat applied to the center of the upper edge.
 * The rectangular plane is represented as a grid of points.
 * The Laplace equation dictates how the heat will transfer from grid point to grid point over time.
 * The program simulates the diffusion of temperature in all points over time.
 */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <omp.h>

#define N 5000 /* grid points */
#define MAXITER 100
#define NTHREADS 8

#define SMALL_CHUNK_SIZE 1
#define LARGE_CHUNK_SIZE 20

// #define DEBUG

double *grid; // the grid and next generation grid
double *new_grid;

double *groundtruth; // saved result for comparison

void init_temperatures(void)
{
    grid = (double *)malloc(N * N * sizeof(double));
    if (!grid)
    {
        printf("Memory allocation error for grid\n");
        exit(1);
    }
    new_grid = (double *)malloc(N * N * sizeof(double));
    if (!new_grid)
    {
        printf("Memory allocation error for new grid\n");
        exit(1);
    }

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
        {
            grid[i * N + j] = 20;
        }
    /* init boundary conditions:
    apply heat on middle part of upper boundary */
    for (int j = N / 3; j < N * 2 / 3; j++)
    {
        grid[0 * N + j] = 100;
    }
}

void print_grid(void)
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            printf("%5.1lf ", grid[i * N + j]);
        }
        printf("\n");
    }
}

int equal_groundtruth(void)
{
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
        {
            if (grid[i * N + j] != groundtruth[i * N + j])
                return 0;
        }
    return 1;
}

void save_groundtruth(void)
{
    groundtruth = (double *)malloc(N * N * sizeof(double));
    if (!groundtruth)
    {
        printf("Memory allocation error for groundtruth result\n");
        exit(1);
    }

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
        {
            groundtruth[i * N + j] = grid[i * N + j];
        }
}

void swap_ptr(double **p1, double **p2)
{
    double *tmp = *p1;
    *p1 = *p2;
    *p2 = tmp;
}

void serial_temp()
{
    int i, j, time;

    for (time = 0; time < MAXITER; time++)
    {
#ifdef DEBUG
        printf("\nIteration %d \n", time);
        print_grid();
#endif
        for (i = 1; i < N - 1; i++) // iterate grid but skip boundary
            for (j = 1; j < N - 1; j++)
            {
                new_grid[i * N + j] = (grid[(i + 1) * N + j] +
                                       grid[(i - 1) * N + j] +
                                       grid[i * N + j + 1] +
                                       grid[i * N + j - 1]) *
                                      0.25;
            }

        // Make new grid to current grid for the next generation
        swap_ptr(&grid, &new_grid);
    }
}

void parallel_temp_different()
{
    for (int time = 0; time < MAXITER; time++)
    {
#pragma omp parallel for num_threads(NTHREADS) collapse(2) schedule(runtime) shared(grid, new_grid)
        for (int i = 1; i < N - 1; i++) // iterate grid but skip boundary
            for (int j = 1; j < N - 1; j++)
            {
                new_grid[i * N + j] = (grid[(i + 1) * N + j] +
                                       grid[(i - 1) * N + j] +
                                       grid[i * N + j + 1] +
                                       grid[i * N + j - 1]) *
                                      0.25;
            }
        // Make new grid to current grid for the next generation
        swap_ptr(&grid, &new_grid);
    }
}

void printTimeForFunction(void (*computeFunction)(), char *versionName) {
    double start = 0, end = 0, time = 0;

    // init the temperatures each time for good measure
    printf("Initialize grid size N=%d\n", N);
    init_temperatures();

    // printf("%s version:...", versionName);
    printf("Start %s with MAXITER = %d\n", versionName, MAXITER);
    start = omp_get_wtime();
    computeFunction();
    end = omp_get_wtime();
    time = end - start;
    printf("%s: %lf\n", versionName, time);
}

int main(int argc, char *argv[]) {
    printTimeForFunction(serial_temp, "Serial");
    save_groundtruth();

    /*
        schimb schedula dinamic la runtime folosind schedule(runtime) in pragma
        apoi trebuie setata schedula prin omp_set_schedule() cu parametri necesari inainte de fiecare apel de functie
    */

    omp_set_schedule(omp_sched_static, SMALL_CHUNK_SIZE);
    printTimeForFunction(parallel_temp_different, "Parallel Static with Chunksize = 1");

    omp_set_schedule(omp_sched_static, LARGE_CHUNK_SIZE);
    printTimeForFunction(parallel_temp_different, "Parallel Static with Chunksize = 20");

    omp_set_schedule(omp_sched_dynamic, SMALL_CHUNK_SIZE);
    printTimeForFunction(parallel_temp_different, "Parallel Dynamic with Chunksize = 1");

    omp_set_schedule(omp_sched_dynamic, LARGE_CHUNK_SIZE);
    printTimeForFunction(parallel_temp_different, "Parallel Dynamic with Chunksize = 20");

    if (!equal_groundtruth())
        printf("!!! Parallel version produces a different result! \n");
    else
        printf("Parallel version produced the same result \n");
}