#include <stdio.h>
#include <stdlib.h>

typedef struct node
{
    int data;
    struct node *next;
} queueNode;

int isEmpty(void);
void put(int val);
int get();