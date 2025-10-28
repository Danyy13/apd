/**
 * Note the difference between measuring wall-clock time and CPU time!
 * 
 * clock_gettime() - measures wallclock time or CPUtime, depending on first argument used
 * clock_gettime(CLOCK_MONOTONIC, &t)
 * clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t); 
 * 
 * clock() - not a portable solution! measures CPU time on unix and wall-clock on windows 
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/* size of dummy workload for threads*/
#define MAXWORK 20000

/* number of working threads */
#define NTHREADS 4

/* thread function 1 - does some dummy counting */
void *pth_fct1(void *dummy)
{
    int count = 0;
    for (int i = 0; i < MAXWORK; i++)
        for (int j = 0; j < MAXWORK; j++)
            count++;
    return NULL;
} /* pth_fct1 */

/* solution 1 - spawns threads executing pth_fct1 */
void solution1(void)
{
    long thread;
    pthread_t *thread_handles;

    thread_handles = malloc(NTHREADS * sizeof(pthread_t));

    for (thread = 0; thread < NTHREADS; thread++)
        pthread_create(&thread_handles[thread], NULL,
                       pth_fct1, NULL);

    for (thread = 0; thread < NTHREADS; thread++)
        pthread_join(thread_handles[thread], NULL);

    free(thread_handles);
}

/* thread function 2 - does dummy counting AND sleeps in between*/
void *pth_fct2(void *dummy)
{
    int count = 0;

    for (int i = 0; i < MAXWORK; i++)
    {
        if (i % 2000 == 0)
            sleep(1);
        for (int j = 0; j < MAXWORK; j++)
            count++;
    }

    return NULL;
} /* pth_fct2 */

/* solution 2 - spawns threads executing pth_fct2 */
void solution2(void)
{
    long thread;
    pthread_t *thread_handles;

    thread_handles = malloc(NTHREADS * sizeof(pthread_t));

    for (thread = 0; thread < NTHREADS; thread++)
        pthread_create(&thread_handles[thread], NULL,
                       pth_fct2, NULL);

    for (thread = 0; thread < NTHREADS; thread++)
        pthread_join(thread_handles[thread], NULL);

    free(thread_handles);
}

#define WALL_CLOCK_TIME_STRING "Wall Clock Time"
#define CPU_TIME_STRING "CPU Time"

double getElapsedTime(struct timespec start, struct timespec finish) {
    double secondsElapsed = finish.tv_sec - start.tv_sec;
    double nanosecondsElapsed = (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    return secondsElapsed + nanosecondsElapsed;
}

void showTimeForFunction(void (*function)(), int timeType, char *nameOfTimeType) {
    struct timespec start, finish;
    double elapsedTime = 0;

    printf("Measuring %s\n", nameOfTimeType);
    printf("Start...\n");
    clock_gettime(timeType, &start);

    function();

    clock_gettime(timeType, &finish);
    elapsedTime = getElapsedTime(start, finish);

    printf("%s = %lf\n", nameOfTimeType, elapsedTime);
}

void showTimeForFunctionUsingClock(void (*function)()) {
    double elapsedTime = 0;
    clock_t startTime = 0, endTime = 0;

    printf("Measuring with clock()\n");
    printf("Start...\n");
    startTime = clock();

    function();

    endTime = clock();
    elapsedTime = ((double)(endTime - startTime)) / CLOCKS_PER_SEC;

    printf("Time using clock() = %lf\n", elapsedTime);
}

int main(void)
{
    /**
     * Experiment1: Measuring Wall-clock time
     * uses clock_gettime  CLOCK_MONOTONIC
     */
    showTimeForFunction(solution1, CLOCK_MONOTONIC, WALL_CLOCK_TIME_STRING);

    showTimeForFunction(solution2, CLOCK_MONOTONIC, WALL_CLOCK_TIME_STRING);

    /**
     *  Experiment2: Measuring CPU time PER PROCESS (all threads)
     *  uses clock_gettime CLOCK_PROCESS_CPUTIME_ID
     */
    
    showTimeForFunction(solution1, CLOCK_PROCESS_CPUTIME_ID, CPU_TIME_STRING);

    showTimeForFunction(solution2, CLOCK_PROCESS_CPUTIME_ID, CPU_TIME_STRING);

    /**
     *  Experiment3: using clock()
     *  CPU time on unix/linux
     *  wall-clock time on windows
     */
    
   showTimeForFunctionUsingClock(solution1);

   showTimeForFunctionUsingClock(solution1);
}

/*
    1. compare TIME1 and TIME2 (wall-clock-times of Solution1 and Solution2)
    TIME1 = 0.408507
    TIME2 = 10.309632
    TIME1 << TIME2. Se observa ca timpul masurat "pe cronometru" (Wall Clock Time) la functia a
doua este cu 10 secunde mai mare decat cel al functiei 1 deoarece apare un sleep deo secunda
la fiecare (i % 2000 == 0), unde i = [0, 20000) => se executa sleep de 10 ori. Aceasta masuratoare
nu tine cont decat de cat timp a trecut de la initierea timer-ului pana la oprire

    2. compare TIME3 and TIME4 (CPU-times of Solution1 and Solution2)
    TIME3 = 1.252613
    TIME4 = 1.036945
    TIME3 > TIME4. Timpuri similare deoarece masuratoare este facuta pe timpul efectiv de lucru
al procesorului, pe "efortul" sau. In cele 10 secunde de sleep din functia a doua procesorul nu
depunde niciun efort, deci timpul acela nu se ia in calcul.

    3. compare TIME1 and TIME3 (wall-clock and CPU-times of Solution1)
    TIME1 = 0.408507
    TIME3 = 1.252613
    TIME1 < TIME3. Rezultatul TIME3 este obtinut insumand CPU time-urile individuale ale
tuturor core-urilor. Programul a rulat pe mai multe core-uri, deci de asta s-a ajuns la acest
rezultat.

    4. compare TIME2 and TIME4 (wall-clock and CPU-times of Solution2)
    TIME2 = 10.309632
    TIME4 = 1.036945
    TIME2 >> TIME4. Desi programul a rulat pe mai multe core-uri, nu sunt destul de multe incat
TIME4 sa depaseasca TIME2 deoarece exista acele sleep-uri
*/