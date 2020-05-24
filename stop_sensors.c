//stop_sensors.c
//Autor: Magdalena Zych
//Data: 11.05.2020

#include "stop_sensors.h"

int main(int argc, char *argv[])
{
    char action[5]="stop";
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
    char buffer[5];
    memset( buffer, 0, sizeof( buffer ) );
    if( recvfrom( socket_, buffer, sizeof( buffer ), 0,( struct sockaddr * ) & from, & sensors_serv_size ) < 0 )
    {
        perror( "recvfrom() ERROR" );
        exit( 1 );
    }
    if(strcmp(buffer, "OK")==0)
      printf( "Udalo sie wylaczyc wszystkie czujniki\n");
    else
      printf( "stop_sensors() ERROR\n");

    shutdown( socket_, SHUT_RDWR );
}
