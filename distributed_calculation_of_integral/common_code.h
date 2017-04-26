#ifndef COMMON_CODE_H
#define COMMON_CODE_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

#define port 51000

typedef struct argument
{
    double left;
    double right;
    long int segments;
    double res;

} Arg;

int set_keepalive_params( int sck, int keepcnt, int keepidle, int keepintvl )
{

    int keepalive = 1;

    if ( setsockopt( sck, SOL_SOCKET, SO_KEEPALIVE, \
            &keepalive, sizeof(keepalive) ) < 0 ||
         setsockopt( sck, IPPROTO_TCP, TCP_KEEPCNT, \
            &keepcnt, sizeof(int) ) < 0 ||
         setsockopt( sck, IPPROTO_TCP, TCP_KEEPIDLE, \
            &keepidle, sizeof(int) ) < 0 ||
         setsockopt( sck, IPPROTO_TCP, TCP_KEEPINTVL, \
            &keepintvl, sizeof(int) )  < 0 )
    {
        return -1;
    }

    return 0;
}
/* This function closes every socket in array of socket descriptors
 * It returns size, if all sockets are closed. In the other case,
 * the function returns the position of the last unclosed descriptor.
*/
int close_connections( int* connections, int size )
{
    int i = 0;
    int last_unclosed = size;

    for ( i = 0; i < size; ++i )
    {
        if ( close( connections[i] ) != 0 )
            last_unclosed = i;
    }

    return last_unclosed;
}

#endif // COMMON_CODE_H
