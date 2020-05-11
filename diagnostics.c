//Autor: Magdalena Zych
//Data: 11.05.2020

#include "diagnostics.h"

int main(int argc, char *argv[])
{
    char action[5]="diag";

    //wyslanie zapytania do modułu z czujnikami
    char sensors_ip[20];
    int sensors_port;
    get_server_parameters(sensors_ip, &sensors_port, 3);

    struct sockaddr_in sensors_serv =
    {
        .sin_family = AF_INET,
        .sin_port = htons( sensors_port )
    };
    if( inet_pton( AF_INET, sensors_ip, & sensors_serv.sin_addr ) <= 0 )
    {
        printf( "ERROR-inet_pton() \n" );
        exit( 1 );
    }

    const int socket_ = socket( AF_INET, SOCK_DGRAM, 0 );
    if( socket_ < 0 )
    {
        printf( "ERROR-socket() \n" );
        exit( 1 );
    }

    socklen_t sensors_serv_size = sizeof( sensors_serv );
    if( sendto( socket_, action, sizeof(action), 0,( struct sockaddr * ) & sensors_serv, sensors_serv_size ) < 0 )
    {
        perror( "ERROR-sendto() \n" );
        exit( 1 );
    }

    //odpowiedź
    struct sockaddr_in from = { };
    union intInBuffer int_buffer;
    memset( int_buffer.buffer, 0, sizeof( int_buffer.buffer ) );
    if( recvfrom( socket_, int_buffer.buffer, sizeof( int_buffer.buffer ), 0,( struct sockaddr * ) & from, & sensors_serv_size ) < 0 )
    {
        perror( "recvfrom() ERROR" );
        exit( 1 );
    }
    printf( "Liczba wyslanych pomiarow: %d\n", int_buffer.intValue);
    shutdown( socket_, SHUT_RDWR );

    //wyslanie zapytania do modułu odbierającego pomiary (serwera)
    char server_ip[20];
    int server_port;
    get_server_parameters(server_ip, &server_port, 2);

    struct sockaddr_in server =
    {
        .sin_family = AF_INET,
        .sin_port = htons(server_port)
    };
    if(inet_pton( AF_INET, server_ip, & server.sin_addr ) <= 0)
    {
        printf( "ERROR-inet_pton() \n" );
        exit( 1 );
    }

    const int server_socket_ = socket( AF_INET, SOCK_DGRAM, 0 );
    if( server_socket_ < 0 )
    {
        printf( "ERROR-socket() \n" );
        exit( 1 );
    }

    socklen_t server_size = sizeof( server );
    if( sendto( server_socket_, action, sizeof(action), 0,( struct sockaddr * ) & server, server_size ) < 0 )
    {
        perror( "ERROR-sendto() \n" );
        exit( 1 );
    }

    //odpowiedź
    struct sockaddr_in server_from = { };
    memset( int_buffer.buffer, 0, sizeof( int_buffer.buffer ) );
    if( recvfrom( server_socket_, int_buffer.buffer, sizeof( int_buffer.buffer ), 0,( struct sockaddr * ) & server_from, & server_size ) < 0 )
    {
        perror( "recvfrom() ERROR" );
        exit( 1 );
    }
    printf( "Liczba odebranych pomiarow: %d\n", int_buffer.intValue);

    shutdown( server_socket_, SHUT_RDWR );
}
