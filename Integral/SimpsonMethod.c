#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>

#include <hwloc.h>

#define N 160000000 // Corresponds to the number
                    // of intervals for separated calculation
                    // divided by 2; N = numOfintervals/2;

typedef struct argument
{
    double left;
    double right;
    long int segments;
    double res;

} Arg;

double f ( double x )
{
    return x * x;
}

void calculate ( Arg* arg )
{

    double from = arg->left;
    double to   = arg->right;
    long int parts = arg->segments;
    double step = ( to - from ) / arg->segments;
    double result = 0;

    result = f( from ) + f( to );
    long int i = 0;

    for ( i = 1; i < parts; i++ )
    {
        if ( i % 2 == 0 )
            result += 2 * f( from + i * step );
        else
            result += 4 * f( from + i * step );
    }

    arg->res = result / 3 * step;
}

// input from, to, threads ( n <= 0 => nCPU used )
int main ( int argc, char** argv )
{
    double from = 0;
    double to = 0;
    int threadsNumber = 0;
//  Input Handling=====================================
    if ( argc != 4 )
    {
        printf( "%s", "Incorrect number of arguments\n" );
        exit( EXIT_FAILURE );
    }

    if ( sscanf( argv[1], "%lf", &from ) == 0 )
    {
        printf( "%s", "Incorrect field: from\n" );
        exit( EXIT_FAILURE );
    }

    if ( sscanf( argv[2], "%lf", &to ) == 0 )
    {
        printf( "%s", "Incorrect field: to\n" );
        exit( EXIT_FAILURE );
    }

    if ( sscanf( argv[3], "%d", &threadsNumber ) == 0 )
    {
        printf( "%s", "Incorrect field: threadsNumber\n" );
        exit( EXIT_FAILURE );
    }

    if ( threadsNumber <= 0 )
        threadsNumber = 1;

    size_t cores = sysconf( _SC_NPROCESSORS_ONLN );

    if ( threadsNumber > cores )
    {
        printf( "%s", "Number of threads exceeded\n" );
        exit( EXIT_FAILURE );
    }

//=======================================================
    double segLeng = ( to - from ) / threadsNumber;

    pthread_t* threads = ( pthread_t* ) malloc( sizeof( pthread_t ) * threadsNumber );
    Arg* args          = ( Arg* ) malloc ( sizeof( Arg ) * threadsNumber );

    if ( ( threads == NULL || args == NULL ) && errno != 0 )
    {
        perror( "malloc" );
        exit( EXIT_FAILURE );
    }

    long int even_N = N / threadsNumber;

    if ( even_N == 0 )
        even_N = 2;
    else
        even_N = 2 * even_N;

    pthread_attr_t attr;
    pthread_attr_init( &attr );
    cpu_set_t cpu_set;

    long int i = 0;
    int border =  cores / 2;

    for ( i = 0; i < threadsNumber; ++i )
    {
        args[i].left     = from + i * segLeng;
        args[i].right    = from + ( i + 1 ) * segLeng;
        args[i].segments = even_N;
        args[i].res      = 0;

        CPU_ZERO( &cpu_set );

        if ( i < border )
            CPU_SET( i * 2, &cpu_set );
        else
            CPU_SET( ( i - border ) * 2 + 1, &cpu_set );

        pthread_attr_setaffinity_np( &attr, sizeof ( cpu_set_t ), &cpu_set );
        errno = pthread_create( &threads[i], &attr, ( void* (*) ( void* ) ) calculate, &( args[i] ) );

        if ( errno != 0 )
        {
            perror( "pthread_create" );
            exit( EXIT_FAILURE );
        }
    }

    double sum = 0;

    for ( i = 0; i < threadsNumber; i++ )
    {
        errno = pthread_join( threads[i], NULL );

        if ( errno != 0 )
        {
            perror( "pthread_join" );
            exit( EXIT_FAILURE );
        }

        sum += args[i].res;
    }

    printf ( "Result is %lf\n", sum );

    pthread_attr_destroy( &attr );
    free( threads );
    free( args );

    exit( EXIT_SUCCESS );
}
