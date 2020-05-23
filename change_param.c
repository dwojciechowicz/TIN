//Autor: Maria Jarek
//Data: 23.05.2020

#include "change_param.h"

int main(int argc, char *argv[])
{
    char action[15] = { };

    strcat(action, "para ");
    for(int i=1; i<3; ++i)
    {
      strcat(action, argv[i]);
      strcat(action, " ");
    }
    //rozbite, aby ostatnim znakiem nie byla spacja ^
    strcat(action, argv[3]);
    printf("%s\n", action);

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

    shutdown( socket_, SHUT_RDWR );

  }
