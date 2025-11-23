#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NUMTHREADS 8

typedef struct binary_tree
{
    int data;
    struct binary_tree *left;
    struct binary_tree *right;
} binary_tree;


// insert as in a binary search tree
binary_tree *insert(binary_tree *tree, int data)
{
    if (tree == NULL)
    {
        binary_tree *new = malloc(sizeof(binary_tree));
        new->data = data;
        new->left = NULL;
        new->right = NULL;
        return new;
    }
    else if (tree->data >= data)
    {
        tree->left = insert(tree->left, data);
    }
    else
    {
        tree->right = insert(tree->right, data);
    }
    return tree;
}


int maximum(int a, int b)
{
    return a >= b ? a : b;
}

// serial version binary tree height
int height(binary_tree *tree)
{
    if (tree == NULL)
        return 0;

    int left, right;

    left = height(tree->left);

    right = height(tree->right);

    return maximum(left, right) + 1;
}

// parallel version binary tree height - TO DO
int height_p(binary_tree *tree, int numThreads)
{
    if(tree == NULL) return 0;

    int left, right;
    if(numThreads == 1) {
        return height(tree);
    }

#pragma omp task shared(left)
{
    left = height_p(tree->left, numThreads / 2);
}

#pragma omp task shared(right)
{
    right = height_p(tree->right, numThreads / 2);
}
#pragma omp taskwait

    return maximum(left, right) + 1;
}

int height_pCaller(binary_tree *tree) {
    int ret = 0;

    omp_set_num_threads(NUMTHREADS);
#pragma omp parallel
    {
#pragma omp single nowait
        ret = height_p(tree, NUMTHREADS);
    }

    return ret;
}

int main() {
    omp_set_num_threads(NUMTHREADS);
    binary_tree *tree = NULL;

    int n = 3000000;
    printf("Working to build a binary tree with %d nodes ... \n", n);
    srand(time(NULL));
    for (int i = 0; i < n; i++)
    {
        int x = rand() % n;
        tree = insert(tree, x);
    }
    printf("Finished building the tree \n");

    double start = 0, serialTime = 0, parallelTime = 0; 
    int h = 0;

    start = omp_get_wtime();
    h = height(tree);
    serialTime = omp_get_wtime() - start;

    printf("Height = %d\tSerial time=%f\n", h, serialTime);

    start = omp_get_wtime();
    h = height_pCaller(tree);
    parallelTime = omp_get_wtime() - start;

    printf("Height = %d\tParallel time=%f\n", h, parallelTime);
    
    printf("Speedup = %lf\n", serialTime / parallelTime);

    return 0;
}