#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <pthread.h>
//#include <sys/types.h>

#define N 58982400L // Corresponds to the number
                    // of intervals for separated calculation
                    // divided by 2; N = numOfintervals/2;
#define TO 100.0
#define FROM 1.0

typedef struct argument
{
    double left;
    double right;
    long int segments;
    double res;

} Arg;

double f ( double x )
{
    return sinl(x) / x;
}

void calculate ( Arg* arg )
{
    double from = arg->left;
    double to   = arg->right;
    long int parts = arg->segments;
    double step = ( to - from ) / (double)arg->segments;
    double result = 0;

    result = f( from ) + f( to );
    long int i = 0;

    for ( i = 1; i < parts; i++ )
    {
        if ( i % 2 == 0 )
            result += 2.0 * f( from + i * step );
        else
            result += 4.0 * f( from + i * step );
    }

    arg->res = result / 3.0 * step;
}

// threads ( n <= 0 => nCPU used )
int main ( int argc, char** argv )
{
    int threadsNumber = 0;
//  Input Handling=====================================
    if ( argc != 2 )
    {
        printf( "%s", "Incorrect number of arguments\n" );
        exit( EXIT_FAILURE );
    }

    if ( sscanf( argv[1], "%d", &threadsNumber ) == 0 )
    {
        printf( "%s", "Incorrect field: threadsNumber\n" );
        exit( EXIT_FAILURE );
    }

    if ( threadsNumber <= 0 )
    {
        printf( "%s", "Negative number of threads\n" );
        exit( EXIT_FAILURE );
    }

    size_t cores = sysconf( _SC_NPROCESSORS_ONLN );

    if ( threadsNumber > cores )
    {
        printf( "%s", "Number of threads exceeded\n" );
        exit( EXIT_FAILURE );
    }

//=======================================================
    double segLeng = ( TO - FROM ) / (double)threadsNumber;

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

    int i = 0;
    int border =  cores / 2;

    for ( i = 0; i < threadsNumber; ++i )
    {
        args[i].left     = FROM + i * segLeng;
        args[i].right    = FROM + ( i + 1 ) * segLeng;
        args[i].segments = even_N;
        args[i].res      = 0;

        errno = pthread_create( &threads[i], NULL, ( void* (*) ( void* ) ) calculate, &( args[i] ) );

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

    free( threads );
    free( args );

    exit( EXIT_SUCCESS );
}
