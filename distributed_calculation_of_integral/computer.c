#define _GNU_SOURCE // sysconf(...) needs this define

#include <math.h>
#include "common_code.h"

#define buff_size 1024

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

int start_calculation( Arg* arg, int cores )
{
    double segLeng = ( arg->right - arg->left ) / (double) cores;

    pthread_t* threads = ( pthread_t* ) malloc( sizeof( pthread_t ) * cores );
    Arg* args          = ( Arg* ) malloc ( sizeof( Arg ) * cores );

    if ( ( threads == NULL || args == NULL ) && errno != 0 )
    {
        perror( "malloc" );
        return EXIT_FAILURE;
    }

    int i = 0;
    long int even_N = 2 * ( arg->segments / cores );

    for ( i = 0; i < cores; ++i )
    {
        args[i].left     = arg->left + i * segLeng;
        args[i].right    = arg->left + ( i + 1 ) * segLeng;
        args[i].segments = even_N;
        args[i].res      = 0;

        errno = pthread_create( &threads[i], NULL, ( void* (*) ( void* ) ) calculate, &( args[i] ) );

        if ( errno != 0 )
        {
            perror( "pthread_create" );
            return EXIT_FAILURE;
        }
    }

    double sum = 0;

    for ( i = 0; i < cores; i++ )
    {
        errno = pthread_join( threads[i], NULL );

        if ( errno != 0 )
        {
            perror( "pthread_join" );
            return EXIT_FAILURE;
        }

        sum += args[i].res;
    }

    arg->res = sum;
    return EXIT_SUCCESS;
}

int main( int argc, char** argv )
{
    if ( argc != 2 )
    {
        fprintf( stderr, "c: %s", "Incorrect fromat\n" );
        exit( EXIT_FAILURE );
    }

    int cores = 0;
    errno = 0;
    // Number of expected computers in LAN
    cores = strtol( argv[1], NULL, 10 );
    if ( errno )
    {
        perror( "c: Incorrect format" );
        exit( EXIT_FAILURE );
    }

    if ( cores <= 0 )
    {
        fprintf( stderr, "%s cores: %d\n", "c: Incorrect format", cores );
        exit( EXIT_FAILURE );
    }
    // Creating UDP receiving socket

    int bc_sck = socket( AF_INET, SOCK_DGRAM, 0 );
    if ( bc_sck == -1 )
    {
        perror( "c: Error happened, creating UDP socket" );
        exit( EXIT_FAILURE );
    }
    // Setting UDP socket parameters
    int ok = 1;
    if ( setsockopt( bc_sck, SOL_SOCKET, SO_REUSEADDR, &ok, sizeof(ok) ) < 0 )
    {
        perror( "c: Error happened, creating UDP socket" );
        exit( EXIT_FAILURE );
    }

    struct sockaddr_in rec_addr;
    bzero( &rec_addr, sizeof(rec_addr) );
    rec_addr.sin_family = AF_INET;
    rec_addr.sin_addr.s_addr = INADDR_ANY;
    rec_addr.sin_port = htons( (unsigned short) port );

    if ( bind( bc_sck, (const struct sockaddr*) &rec_addr, sizeof(rec_addr) ) )
    {
        perror( "c: Error happened in bind(...)" );
        exit( EXIT_FAILURE );
    }

    char buffer[buff_size];
    bzero( buffer, buff_size );
    unsigned int received = 0;
    socklen_t rec_len = buff_size;
    // Receiving message in blocking recvfrom(...)
    // Process is waiting for any notification on the certain port (51000)
    fprintf( stderr, "c: waiting on port %d\n", port);

    if ( ( received = recvfrom( bc_sck,
                                ( void* ) buffer,
                                buff_size, 0,
                                ( struct sockaddr* )&rec_addr,
                                &rec_len ) ) < 0 )
    {
        fprintf( stderr, "Error %s\n", strerror( errno ) );
        exit( EXIT_FAILURE );
    }

    rec_addr.sin_port = htons( (unsigned short) port );
    fprintf( stderr, "c: msg -- %s", buffer );
    fprintf( stderr, "c: ip --  %s, port -- %d\n", inet_ntoa( rec_addr.sin_addr ),
                                           ntohs( rec_addr.sin_port ) );
    // UDP socket is of no use anymore. It should be closed.
    close( bc_sck );
    // Initiating a TCP connection with distributor
    // Creating a TCP socket
    int tcp_sck = socket ( AF_INET, SOCK_STREAM, 0 );
    if ( tcp_sck == -1 )
    {
        perror( "c: Error happened, creating TCP socket" );
        exit( EXIT_FAILURE );
    }
    // Setting TCP socket parameters
    if ( set_keepalive_params( tcp_sck, 2, 5, 5 ) == -1 )
    {
        perror( "c: setsockopt() error" );
        close( tcp_sck );
        exit( EXIT_FAILURE );
    }
    // Connecting to the distributor
    if ( connect( tcp_sck, ( const struct sockaddr* ) &rec_addr, sizeof(rec_addr) ) < 0 )
    {
		perror( "c: Connection error" );
		close( tcp_sck );
        exit( EXIT_FAILURE );
	}

	fprintf( stderr, "c: %s", "Connection established\n" );
	// Sending the number of cores

    if ( send( tcp_sck, (void*) &cores, sizeof(cores), 0 ) < 0 )
    {
        perror( "c: send(...) error" );
        close( tcp_sck );
        exit( EXIT_FAILURE );
    }
    // Waiting for the data
    Arg data;
    int bytes_recv = 0;

    if ( ( bytes_recv = recv( tcp_sck, (void*) &data, sizeof(Arg), MSG_WAITALL ) ) <= 0 )
    {
        if ( bytes_recv != 0 )
            perror( "c: recv(...) error" );
        else
            fprintf( stderr, "c: %s", "Didn't received data from distributor\n" );
        close( tcp_sck );
        exit( EXIT_FAILURE );
    }
    // Calculations
    fprintf( stderr, "c: %s", "Calculation started\n" );
    start_calculation( &data, cores );
    int bytes_sent = 0;
    // Sending back the result
    if ( ( bytes_sent = send( tcp_sck, (void*) &(data.res), sizeof(data.res), 0 ) ) <= 0 )
    {
        perror( "c: send(...) error" );
        close( tcp_sck );
        exit( EXIT_FAILURE );
    }

    fprintf( stderr, "c: %s", "Answer sent\n" );

    close( tcp_sck );
    exit( EXIT_SUCCESS );
}
