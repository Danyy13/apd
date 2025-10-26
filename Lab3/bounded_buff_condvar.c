/*
File bounded_buff_condvar.c
Bounded buffer -  a Circular array of fixed size
is used as a Queue between a number of Producers and Consumers.
Version: with locks and condition variables
*/
#include <stdio.h>
#include <pthread.h>


#define REPEAT 5    /* each thread is doing REPEAT operations  */
#define NUM_THREADS 4

#define BUFFER_SIZE 4 /* size of buffer = Queue */

/* The buffer containing the Queue */
int b[BUFFER_SIZE];

/* Index of queue tail (where to put next) */
int in = 0;
/* Index of queue head (where to get next) */
int out = 0;

/* mutex lock protecting access to the buffer */
pthread_mutex_t lock;

/* condition variable to signal when buffer
is not empty - wakes up a waiting consumer  */
pthread_cond_t not_empty_cv;

/* condition variable to signal when buffer
is not full - wakes up a waiting producer */
pthread_cond_t not_full_cv;

void init_synchro()
{
    /* Initialize mutex */
    pthread_mutex_init(&lock, NULL);
    /* Initialize cond vars */
    pthread_cond_init(&not_empty_cv, NULL);
    pthread_cond_init(&not_full_cv, NULL);
}

void destroy_synchro()
{
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&not_empty_cv);
    pthread_cond_destroy(&not_full_cv);
}

/* 
Add one value into the Queue.
Used by Producer threads.
If Queue is full, ensures that thread waits. 
*/
void enqueue(int value)
{
    pthread_mutex_lock(&lock);

    /* while queue is full, wait */
    /* need a while loop, not a simple if !!! */
    while ((in + 1) % BUFFER_SIZE == out)
        pthread_cond_wait(&not_full_cv, &lock);

    /* put in queue */
    b[in] = value;
    in = (in + 1) % BUFFER_SIZE;

    /* signal that buffer is not empty */
    pthread_cond_signal(&not_empty_cv);
    pthread_mutex_unlock(&lock);
}

/* 
Pop one value from the Queue and return it.
Used by Consumer threads.
If Queue is empty, ensures that thread waits.
*/
int dequeue()
{
    pthread_mutex_lock(&lock);

    /* while queue is empty, wait */
     /* need a while loop, not a simple if !!! */
    while (out == in)
        pthread_cond_wait(&not_empty_cv, &lock);

    /* take out an element */
    int tmp = b[out];
    out = (out + 1) % BUFFER_SIZE;

    /* signal that buffer is not full   */
    pthread_cond_signal(&not_full_cv);

    /* exit critical section */
    pthread_mutex_unlock(&lock);

    return tmp;
}

/* 
thread function for producer threads.
thread 0 produces: 0 1 2 3 4
thread 2 produces: 20 21 22 23 24 
*/
void *producer(void *t)
{
    int i;
    int my_id = *(int *)t;

    for (i = 0; i < REPEAT; i++)
    {
        enqueue(i + my_id * 10);
    }
    pthread_exit(NULL);
}

/* thread function for consumer threads */
void *consumer(void *t)
{
    int i;
    int my_id = *(int *)t;

    for (i = 0; i < REPEAT; i++)
    {
        int rez = dequeue();
        printf("Consumer thread %d got %d \n", my_id, rez);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    /* Init cond var and mutex */
    init_synchro();

    /* Create threads */
    /* odd thread ranks are consumers, even ranks are producers */
    for (int i = 0; i < NUM_THREADS; i++)
    {
        ids[i] = i;
        if (i % 2 == 0)
            pthread_create(&threads[i], NULL, producer, (void *)&ids[i]);
        else
            pthread_create(&threads[i], NULL, consumer, (void *)&ids[i]);
    }

    /* Wait for all threads to complete */
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    /* Clean up */
    destroy_synchro();
}
