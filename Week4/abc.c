/* File:  abc.c
Creates 3 threads.
First thread repeatedly writes A.
Second thread repeatredly writes B.
Third thread repeatedly writes C.
The main thread repeatedly writes newline.

Add synchronization such that the output is always in the form:
on each new line there is a random permutation of A,B,C.
You can add any synchronization that you want, but you 
can not change the existing printf's or add new printf's


For example, the output can be:
ABC
BAC
ABC
BCA
...
*/

/*
    Fiecare functie Hello are bariera de litere dupa printare
pentru a asigura printarea de litere cum trebuie, iar dupa asta
se asteapta la bariera pentru '\n'-ul din 
    In main se asteapta intai printarea tuturor literelor folosind
bariera de litere, apoi se printeaza
    Daca bariera de dupa print-ul din main nu exista, nu poate fi
semnalata printarea '\n'-ului, asa ca se pot printa caractere
    Daca newLineBarrier vine inaintea printarilor din functiile de
litere, atunci programul se blocheaza deoarece nicio bariera nu va
putea fi deblocata. Literele asteapta un '\n', iar main-ul asteapta
literele
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_barrier_t letterBarrier, newLineBarrier;

#define REPEAT 100
#define LETTER_THREAD_COUNT 3
#define THREAD_COUNT 4

/* Thread function A */
void *HelloA(void *dummy)
{
    for (int i = 0; i < REPEAT; i++) {
        printf("A");
        pthread_barrier_wait(&letterBarrier);
        pthread_barrier_wait(&newLineBarrier);
    }
        
    return NULL;
}

/* Thread function B */
void *HelloB(void *dummy)
{
    for (int i = 0; i < REPEAT; i++) {
        printf("B");
        pthread_barrier_wait(&letterBarrier);
        pthread_barrier_wait(&newLineBarrier);
    }

    return NULL;
}

/* Thread function C */
void *HelloC(void *dummy)
{
    for (int i = 0; i < REPEAT; i++) {
        printf("C");
        pthread_barrier_wait(&letterBarrier);
        pthread_barrier_wait(&newLineBarrier);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t thread_handleA, thread_handleB, thread_handleC;
    
    pthread_barrier_init(&letterBarrier, NULL, THREAD_COUNT);
    pthread_barrier_init(&newLineBarrier, NULL, THREAD_COUNT);

    pthread_create(&thread_handleA, NULL, HelloA, NULL);
    pthread_create(&thread_handleB, NULL, HelloB, NULL);
    pthread_create(&thread_handleC, NULL, HelloC, NULL);

    for (int i = 0; i < REPEAT; i++) {
        pthread_barrier_wait(&letterBarrier);
        printf("\n");
        pthread_barrier_wait(&newLineBarrier);
    }

    pthread_join(thread_handleA, NULL);
    pthread_join(thread_handleB, NULL);
    pthread_join(thread_handleC, NULL);

    pthread_barrier_destroy(&letterBarrier);
    pthread_barrier_destroy(&newLineBarrier);

    return 0;
}
