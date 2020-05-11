//Autor: Magdalena Zych
//Data: 11.05.2020

#include "stop_sensors.h"

int main(int argc, char *argv[])
{
  char action[5]="stop";
  if(strcmp(action, "stop")==0)
    printf("Zatrzymanie czujnikow\n");

    char server_ip[20];
    int server_port;
    get_server_parameters(server_ip, &server_port, 3);

    struct sockaddr_in server =
    {
        .sin_family = AF_INET,
        .sin_port = htons( server_port )
    };
    if( inet_pton( AF_INET, server_ip, & server.sin_addr ) <= 0 )
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

    socklen_t server_size = sizeof( server );
    if( sendto( socket_, action, sizeof(action), 0,( struct sockaddr * ) & server, server_size ) < 0 )
    {
        perror( "ERROR-sendto() \n" );
        exit( 1 );
    }
    shutdown( socket_, SHUT_RDWR );
}
