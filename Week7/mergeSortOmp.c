#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

/* number of threads */
#define NUMTHREADS 8

/* size of array */
#define N 1000000

// marimea array-ului pentru care algoritmul sa treaca de la paralel la serial
// #define SERIAL_SIZE 5000
int serialSizes[] = {100, 1000, 2500, 5000, 7500, 10000, 25000, 50000, 75000, 100000, 500000, 0};
int serialSizesIndex = 0;

/* if DEBUG prints arrays */
// #define DEBUG

void merge(int arr[], int left, int mid, int right) {
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;

    int *L = (int *)malloc(n1 * sizeof(int));
    int *R = (int *)malloc(n2 * sizeof(int));

    for (i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    i = 0; j = 0; k = left;

    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }

    free(L);
    free(R);
}

void mergeSort_serial(int arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort_serial(arr, left, mid);
        mergeSort_serial(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

void mergeSort_parallelSections(int arr[], int left, int right, int numThreads) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        if(numThreads == 1) {
            mergeSort_serial(arr, left, right);
            return;
        }

    #pragma omp parallel sections shared(arr) firstprivate(left, mid, right)
        {
    #pragma omp section
    {
            mergeSort_parallelSections(arr, left, mid, numThreads / 2);
            // printf("Thread %d of %d working on it\n", omp_get_thread_num(), omp_get_num_threads());
    }
    #pragma omp section
    {
            mergeSort_parallelSections(arr, mid + 1, right, numThreads / 2);
            // printf("Thread %d of %d working on it\n", omp_get_thread_num(), omp_get_num_threads());
    }
        }

        merge(arr, left, mid, right);
    }
}

void mergeSort_parallelTasks(int arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        if(right - left < serialSizes[serialSizesIndex]) {
            mergeSort_serial(arr, left, right);
            return;
        }

    #pragma omp task shared(arr)
    {
        mergeSort_parallelTasks(arr, left, mid);
        // printf("Thread %d of %d working on it\n", omp_get_thread_num(), omp_get_num_threads());
    }
    
    #pragma omp task shared(arr)
    {
        mergeSort_parallelTasks(arr, mid + 1, right);
        // printf("Thread %d of %d working on it\n", omp_get_thread_num(), omp_get_num_threads());
    }

    #pragma omp taskwait

        merge(arr, left, mid, right);
    }
}

void mergeSort_parallelTasksCaller(int arr[], int size) {
    omp_set_num_threads(NUMTHREADS);
#pragma omp parallel
    {
#pragma omp single nowait
        mergeSort_parallelTasks(arr, 0, size - 1);
    }
}

/* Additional functions */
void print_array(int arr[], int size);
void generate_array(int arr[], int size);
void copy_array(int arr[], int copy[], int size);
int is_sorted(int arr[], int size);

/* Comparator for qsort in stdlib */
int compare(const void* a, const void* b) {
   return (*(int*)a - *(int*)b);
}

int main() {
    double start, run_time, serialTime;

    int *array = (int *)malloc(N * sizeof(int));
    int *copy = (int *)malloc(N * sizeof(int));

    if ((array == NULL) || (copy == NULL)) {
        printf("Not enough memory \n");
        exit(1);
    }

    generate_array(array, N);
#ifdef DEBUG
    print_array(array, N);
#endif
    copy_array(array, copy, N); // store original for another sorting

    printf("Serial MergeSort ... \n");
    start = omp_get_wtime();
    mergeSort_serial(array, 0, N - 1);
    serialTime = omp_get_wtime() - start;
    printf("Serial time = %lf seconds\n ", serialTime);
#ifdef DEBUG
    print_array(array, N);
#endif

    copy_array(copy, array, N); // restore original array for new sorting

    printf("Library function qsort ... \n");
    start = omp_get_wtime();
    qsort(array, N, sizeof(int), compare);
    run_time = omp_get_wtime() - start;
    printf("qsort time = %lf seconds\n ", run_time);
#ifdef DEBUG
    print_array(array, N);
#endif

    printf("Parallel MergeSort with sections... \n");
    start = omp_get_wtime();
    mergeSort_parallelSections(array, 0, N - 1, NUMTHREADS);
    run_time = omp_get_wtime() - start;
    printf("Parallel sections time = %lf seconds\tSpeedup: %lf\n", run_time, serialTime / run_time);
#ifdef DEBUG
    print_array(array, N);
#endif

    copy_array(copy, array, N); // restore original array for new sorting

    while(serialSizes[serialSizesIndex] != 0) {
        printf("Parallel MergeSort with tasks... \n");
        start = omp_get_wtime();
        mergeSort_parallelTasksCaller(array, N);
        run_time = omp_get_wtime() - start;
        printf("Parallel tasks time = %lf seconds\tSerial size: %d\tSpeedup: %lf\n", run_time, serialSizes[serialSizesIndex], serialTime / run_time);
    #ifdef DEBUG
        print_array(array, N);
    #endif

        copy_array(copy, array, N); // restore original array for new sorting

        serialSizesIndex++;
    }

    free(array);
    free(copy);

    return 0;
}

/* Function to print an array */
void print_array(int arr[], int size) {
    for (int i = 0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

/* Function to generate random array */
void generate_array(int arr[], int size) {
    printf("size=%d \n", size);
    for (int i = 0; i < size; i++) {
        arr[i] = rand() % size + 1;
    }
}

/* Function to copy an array */
void copy_array(int arr[], int copy[], int size) {
    for (int i = 0; i < size; i++)
        copy[i] = arr[i];
}

/* Function to check if array is sorted */
int is_sorted(int arr[], int size) {
    for (int i = 0; i < size - 1; i++)
        if (arr[i] > arr[i + 1])
            return 0;
    return 1;
}