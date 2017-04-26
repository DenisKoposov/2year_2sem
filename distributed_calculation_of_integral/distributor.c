#include "common_code.h"

#define FROM 1.0 // lower and upper limits of integration
#define TO 100.0

#define N 58982400L // This number is to define
                    // the partion of the interval (FROM; TO)
#define MAX_COMPUTERS_IN_LAN 256

int main ( int argc, char** argv )
{
    if ( argc != 2 )
    {
        fprintf( stderr, "d: %s", "Incorrect fromat\n" );
        exit( EXIT_FAILURE );
    }

    int expected_computers = 0;
    errno = 0;
    // Number of expected computers in LAN
    expected_computers = strtol( argv[1], NULL, 10 );
    if ( errno )
    {
        perror( "d: Incorrect format" );
        exit( EXIT_FAILURE );
    }

    if ( expected_computers > MAX_COMPUTERS_IN_LAN )
    {
        fprintf( stderr, "%s %d\n",
        "d: Sorry, you are demanding too much, expected_computers <=",
         MAX_COMPUTERS_IN_LAN );

        exit( EXIT_FAILURE );
    }
    // Creating TCP listening socket
    int tcp_sck = socket ( AF_INET, SOCK_STREAM, 0 );
    if ( tcp_sck == -1 )
    {
        perror( "d: Error happened, creating TCP socket" );
        exit( EXIT_FAILURE );
    }
    // Binding TCP socket to the certain port (51000)
    struct sockaddr_in tcp_addr;
    bzero( &tcp_addr, sizeof(tcp_addr) );
    tcp_addr.sin_family = AF_INET;
    tcp_addr.sin_addr.s_addr = INADDR_ANY;
    tcp_addr.sin_port = htons( (unsigned short) port );

    if ( bind( tcp_sck, (struct sockaddr*) &tcp_addr, sizeof(tcp_addr) ) < 0 )
    {
		perror( "d: bind(...) error" );
		close( tcp_sck );
		exit( EXIT_FAILURE );
	}
    // Listening
	if ( listen( tcp_sck, MAX_COMPUTERS_IN_LAN ) < 0 )
	{
		perror( "d: listen(...) error" );
		close( tcp_sck );
		exit( EXIT_FAILURE );
	}
    // Creating UDP broadcasting socket
    int bc_sck = socket( AF_INET, SOCK_DGRAM, 0 );
    if ( bc_sck == -1 )
    {
        perror( "d: Error happened, creating UDP socket" );
        close( tcp_sck );
        exit( EXIT_FAILURE );
    }
    // Send broadcasting invitations
    int ok = 1;
    setsockopt( bc_sck, SOL_SOCKET, SO_BROADCAST, &ok, sizeof(ok) );
    struct sockaddr_in bc_addr;
    bzero( &bc_addr, sizeof(bc_addr) );

    bc_addr.sin_family = AF_INET;
    bc_addr.sin_addr.s_addr = INADDR_BROADCAST;
    bc_addr.sin_port = htons( (unsigned short) port );

    const char* msg = "invitation\n";
    int bytes_sent = 0;

    if ( ( bytes_sent = sendto( bc_sck, msg, strlen(msg), 0, (const struct sockaddr*) &bc_addr, sizeof( bc_addr ) ) ) < 0 )
    {
        perror( "d: Error happened, sending message" );
        close( tcp_sck );
        close( bc_sck );
        exit( EXIT_FAILURE );
    }

    fprintf( stderr, "d: invitation sent\n" );
    // Closing UPD broadcasting socket
    close( bc_sck );
    // Accepting connections
    int connection[MAX_COMPUTERS_IN_LAN];
    int connected = 0;
    int i = 0;
    /* Try to accept more connections, up to 30
     */
    while ( i < expected_computers )
    {
        if ( ( connection[connected] = accept( tcp_sck, NULL, NULL ) ) < 0 )
        {
            i++;
            continue;
        }

        if ( set_keepalive_params( connection[connected], 2, 5, 5 ) == -1 )
        {
            perror( "d: setsockopt() error" );
            close( tcp_sck );
            close_connections( connection, connected );
            exit( EXIT_FAILURE );
        }

        i++;
        connected++;
    }

    close( tcp_sck );
    fprintf( stderr, "d: %d connected\n", connected );

    if ( connected == 0 )
    {
        fprintf( stderr, "%s", "No computers available\n" );
        exit( EXIT_FAILURE );
    }
    // Receiving numbers of cores
    int kernels[MAX_COMPUTERS_IN_LAN];
    int sum = 0;

    for ( i = 0; i < connected; i++ )
    {
        if ( recv( connection[i], (void*) &kernels[i], sizeof(int), 0 ) < 0 )
        {
            perror( "d: Error happened, receiving message" );
            close_connections( connection, connected );
            exit( EXIT_FAILURE );
        }
        sum += kernels[i];
    }

    double interval = ( TO - FROM ) / ( (double) sum );
    int distributed = 0;
    Arg data;
    // Sending data
    for ( i = 0; i < connected; i++ )
    {
        data.left = FROM + distributed * interval;
        data.right = FROM + ( distributed + kernels[i] ) * interval;
        data.segments = 2 * kernels[i] * N / sum;
        data.res = 0;

        if ( send( connection[i], (void*) &data, sizeof(data), 0 ) < 0 )
        {
            perror( "d: Error happened, sending message" );
            close_connections( connection, connected );
            exit( EXIT_FAILURE );
        }

        distributed += kernels[i];
    }
    // Receiving data
    double result = 0;
    double tmp = 0;
    int code = 0;

    for ( i = 0; i < connected; i++ )
    {
        if ( ( code = recv( connection[i], (void*) &tmp, sizeof(tmp), MSG_WAITALL ) ) <= 0 )
        {
            if ( code != 0 )
                perror( "d: Error happened, receiving message" );
            else
                fprintf( stderr, "d: %s\n", "A computer has teared the connection" );

            close_connections( connection, connected );
            exit( EXIT_FAILURE );
        }

        result += tmp;
    }
    // Printing out the result
    fprintf( stdout, "d: The result is %lf\n", result );
    exit( EXIT_SUCCESS );
}
