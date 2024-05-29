#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <math.h>
#include <unistd.h>
#include <sched.h>

int array[65536];
int operation_count = 0;

void calculate_Sqrt(void *arg)
{
    int *thread_id = (int *)arg;
    int start = *thread_id * 16384;
    
    for(int i = start; i < start + 16384; i++)
    {
        array[i] = sqrt(array[i]);
        operation_count++;
    }
} 

void *populate_A(void *arg)
{
    int *thread_id = (int *)arg;
    int start = *thread_id * 16384;
    
    for(int i = start; i < start + 16384; i++)
    {
        array[i] = rand();
    }
    calculate_Sqrt(arg);
}

      

int main( int argc, char **argv ) {

    struct timeval start, end;
    cpu_set_t cpu_s[4];
    
    pthread_t t[4];
    int thread_id[4];
    
    pthread_t t2[4];
    int thread_id2[4];


    printf("Without affinity \n");

    gettimeofday( &start, NULL );
//call your function here

    for(int i = 0; i < 4; i++)
    {
        thread_id[i] = i;
        pthread_create(&t[i], NULL, populate_A, (void *)&thread_id[i]);
    }
    for(int i = 0; i < 4; i++)
    {
        pthread_join(t[i],NULL);
    }

    for(int i = 0; i < 65536; i++)
    {
        printf("%d    " ,array[i]);
    }

    gettimeofday( &end, NULL );

    /* convert time to Gflop/s */
    double seconds = (end.tv_sec - start.tv_sec) +
            1.0e-6 * (end.tv_usec - start.tv_usec);
    double Gflops = 2e-9*operation_count/seconds;


    // wih affinity
    printf("With affinity \n");
    operation_count = 0;
    gettimeofday( &start, NULL );
    for(int i = 0; i < 4; i++)
    {
        thread_id2[i] = i;
        CPU_ZERO(&cpu_s[i]);
        CPU_SET(i, &cpu_s[i]);

        pthread_create(&t2[i], NULL, populate_A, (void *)&thread_id2[i]);
        int result = pthread_setaffinity_np(t2[i], sizeof(cpu_set_t), &cpu_s[i]);
    }
    for(int i = 0; i < 4; i++)
    {
        pthread_join(t2[i],NULL);
    }
    for(int i = 0; i < 65536; i++)
    {
        printf("%d    " ,array[i]);
    }
    gettimeofday( &end, NULL );
    /* convert time to Gflop/s */
    double seconds2 = (end.tv_sec - start.tv_sec) +
            1.0e-6 * (end.tv_usec - start.tv_usec);
    double Gflops2 = 2e-9*operation_count/seconds;

    printf( " \n Performance in Gflops without affinity %.4f Gflop/s", Gflops );
    printf( " \n Performance in Gflops %.4f Gflop/s with affinity \n", Gflops2 );
    printf( " \n Variance is %.4f \n", (Gflops2 - Gflops)/Gflops2 * 100);
    
    return 0;
}