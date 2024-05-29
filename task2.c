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
#include <semaphore.h>

#define array_size 16


int array[array_size][array_size];
int array2[array_size][array_size];

pthread_mutex_t mutex;
int operation_count = 0;
int total_sum = 0;

//semaphore_t sem;
int turn = 0;

struct data
{
    int x;
    int y;
    int power;
};
int sum(int start, int end, int power);


void *populate(void* arg)
{
    struct data *d = (struct data *)arg;
    int start = d->x;
    int end = d->y;
    int power = d->power;
    int size = pow(2, power);
    
    for(int i = start; i < start + size; i++)
    {
        for(int j = end; j < end+size; j++)
        {
            array[i][j] = rand() % 8000 + 8000;            
        }
    }
    sum(start, end, power);
}

int sum(int start, int end, int power)
{
    int size = pow(2, power);
    for(int i = start; i < start+size; i++)
    {
        for(int j = end; j < end+size; j++)
        {
            total_sum += array[i][j];
            operation_count++;
        }
    }
}


// writing function for version 2

void sum_b(int start, int end);

void*  populate_b(void* arg)
{
        struct data *d = (struct data *)arg;
        int start = d->x;
        int end = d->y;

    

    while(start < 16)
    {
        pthread_mutex_lock(&mutex);
        for(int i = start; i < start + 4; i++)
        {
            for(int j = 0; j < end; j++)
            {
                array2[i][j] = rand() % 8000 + 8000;
            }
        }
        sum_b(start, end);
        start += 8;
        pthread_mutex_unlock(&mutex);
    }
}

void sum_b(int start, int end)
{
    for(int i = start; i < start + 4; i++)
    {
        for(int j = 0; j < end; j++)
        {
            total_sum += array2[i][j];
            operation_count++;
        }
    }
}

int main( int argc, char **argv ) {

    struct timeval start, end;
    // allocating memory for the array dynamically

    pthread_mutex_init(&mutex, NULL);

    gettimeofday( &start, NULL );

    struct data d[64]; // for 2^2
    pthread_t t[64];
    
    struct data d2[16]; // for 2^4
    pthread_t t2[16];
    
    struct data d3[4]; // for 2^8
    pthread_t t3[4];
    int x = 0;
    int y = 0;


    for(int i = 0; i < 64; i++)
    {
        d[i].x = x;
        d[i].y = y;
        d[i].power = 2;
        pthread_create(&t[i], NULL, populate, &d[i]);
        x += 2;
        if(x == 16)
        {
            x = 0;
            y += 2;
        }
    }

    for(int i = 0; i < 64; i++)
    {
        pthread_join(t[i], NULL);
    }

    for(int i = 0; i < 16; i++)
    {
        for(int j = 0; j < 16; j++)
        {
            printf("%d ", array[i][j]);
        }
    }

    gettimeofday( &end, NULL );
    printf("Sum of all elements in the array is %d \n", total_sum);

    double seconds = (end.tv_sec - start.tv_sec) +
            1.0e-6 * (end.tv_usec - start.tv_usec);
    double Gflops = 2e-9*operation_count/seconds;


    printf( " \n Performance in Gflops without affinity %.6f Gflop/s", Gflops );
    
    // code for 2^4
    printf("-------------------For 2^4-------------------\n");
    gettimeofday( &start, NULL );
    x = 0;
    y = 0;

    operation_count = 0;
    for(int i = 0; i < 16; i++)
    {
        d2[i].x = x;
        d2[i].y = y;
        d2[i].power = 4;
        pthread_create(&t2[i], NULL, populate, &d2[i]);
        x += 4;
        if(x == 16)
        {
            x = 0;
            y += 4;
        }
    }

    for(int i = 0; i < 16; i++)
    {
        pthread_join(t2[i], NULL);
    }

    for(int i = 0; i < 16; i++)
    {
        for(int j = 0; j < 16; j++)
        {
            printf("%d   ", array[i][j]);
        }
        printf("\n");
    }

    gettimeofday( &end, NULL );
    printf("Sum of all elements in the array is %d \n", total_sum);
    seconds = (end.tv_sec - start.tv_sec) +
            1.0e-6 * (end.tv_usec - start.tv_usec);
    double Gflops2 = 2e-9*operation_count/seconds;


// code for 2^3

    printf("-------------------For 2^3-------------------\n");
    gettimeofday( &start, NULL );
    x = 0;
    y = 0;

    operation_count = 0;
    for(int i = 0; i < 4; i++)
    {
        d3[i].x = x;
        d3[i].y = y;
        d3[i].power = 3;
        pthread_create(&t3[i], NULL, populate, &d3[i]);
        x += 8;
        if(x == 16)
        {
            x = 0;
            y += 8;
        }
    }

    for(int i = 0; i < 4; i++)
    {
        pthread_join(t3[i], NULL);
    }

    for(int i = 0; i < 16; i++)
    {
        for(int j = 0; j < 16; j++)
        {
            printf("%d   ", array[i][j]);
        }
        printf("\n");
    }

    gettimeofday( &end, NULL );

    printf("Sum of all elements in the array is %d \n", total_sum);
    seconds = (end.tv_sec - start.tv_sec) +
            1.0e-6 * (end.tv_usec - start.tv_usec);
    double Gflops3 = 2e-9*operation_count/seconds;

    printf( " \n Performance in Gflops for 2*2 %.6f Gflop/s", Gflops );
    printf( " \n Performance in Gflops for 4*4 %.6f Gflop/s", Gflops2 );
    printf( " \n Performance in Gflops without 8*8 %.6f Gflop/s", Gflops3 );

    gettimeofday( &start, NULL );
    int block_size = 4;
    int ends = 16;
    operation_count = 0;
    data d4[2];

    total_sum = 0;
    pthread_t t4[2];

        d4[0].x = 0;
        d4[0].y = ends;

        d4[1].x = 4;
        d4[1].y = ends;

        pthread_create(&t4[0], NULL, populate_b, &d4[0]);
        pthread_create(&t4[1], NULL, populate_b, &d4[1]);
        pthread_join(t4[0], NULL);
        pthread_join(t4[1], NULL);
    gettimeofday( &end, NULL );
    
    seconds = (end.tv_sec - start.tv_sec) +
            1.0e-6 * (end.tv_usec - start.tv_usec);
    double Gflops4 = 2e-9*operation_count/seconds;

    printf( " \n Performance in Gflops for cyclic %.6f Gflop/s", Gflops4 );
    
    return 0;
}    