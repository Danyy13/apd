/*
Queue implementation with linked list
*/
#include "queue.h"

queueNode *head = NULL;
queueNode *tail = NULL;

int isEmpty(void)
{
    return (head == NULL);
}

void put(int val)
{
    struct node *new = malloc(sizeof(struct node));
    new->data = val;
    new->next = NULL;

    if (!isEmpty())
    {
        tail->next = new;
        tail = new;
    }
    else
    {
        head = new;
        tail = new;
    }
}

int get()
{
    if (isEmpty())
    {
        printf("Error! empty queue \n");
        exit(1);
    }
    struct node *oldhead = head;
    int rez = oldhead->data;
    head = head->next;
    if (head == NULL)
        tail = NULL;
    free(oldhead);
    return rez;
}

/* just a  driver program to test the queue with linked list */
// int main(int argc, char *argv[])
// {
//     for (int i = 0; i < 5; i++)
//         put(i);

//     while (!isEmpty())
//         printf("got %d out of queue \n", get());
// }