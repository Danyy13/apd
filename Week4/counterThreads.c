#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/*
	Minimum: MAX_COUNT - daca mereu se face load si increment dar nu si store
	Maximum: NUM_THREADS * MAX_COUNT
*/

#define NUM_THREADS 2
#define MAX_COUNT 1000000

#define MUTEX

#ifdef MUTEX
	pthread_mutex_t count_mutex;
#endif

int count = 0;

void* inc_count(void* t) {
	// int i;
	int my_id = *(int*)t;

	for (int i = 0; i < MAX_COUNT; i++) {
	#ifdef MUTEX
		pthread_mutex_lock(&count_mutex);
	#endif
		count++;
	#ifdef MUTEX
		pthread_mutex_unlock(&count_mutex);
	#endif
	}
	
	printf("Thread %d incremented count to %d \n", my_id, count);
	return NULL;
}

int main() {
	pthread_t threads[NUM_THREADS];
	int ids[NUM_THREADS];

#ifdef MUTEX
	pthread_mutex_init(&count_mutex, NULL);
#endif

	for (int i = 0; i < NUM_THREADS; i++)
	{
		ids[i] = i;
		pthread_create(&threads[i], NULL, inc_count, (void*)&ids[i]);
	}

	for (int i = 0; i < NUM_THREADS; i++)
	{
		pthread_join(threads[i], NULL);
	}

	printf("Final value of count = %d \n", count);

#ifdef MUTEX
	pthread_mutex_destroy(&count_mutex);
#endif

	return 0;
}