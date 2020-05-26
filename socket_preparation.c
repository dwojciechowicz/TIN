//sensors_func.h
//Autorzy: Magdalena Zych
//Data: 25.05.2020

#include "socket_preparation.h"

const int prepare_socket(int server_port, char* server_ip, bool udplite, struct sockaddr_in * serwer)
{
  serwer->sin_family = AF_INET;
  serwer->sin_port = htons( server_port );

  if( inet_pton( AF_INET, server_ip, &(serwer->sin_addr) ) <= 0 )
  {
      perror( "inet_pton() ERROR" );
      exit( 1 );
  }

  int socket_int;
  if(udplite)
  {
    socket_int = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDPLITE);
  }
  else
  {
    socket_int = socket( AF_INET, SOCK_DGRAM, 0);
  }
  const int socket_=socket_int;

  if(( socket_ ) < 0 )
  {
      perror( "socket() ERROR" );
      exit( 2 );
  }
}

const int prepare_server(socklen_t *len_addr, int server_port, char* server_ip, bool udplite, struct sockaddr_in * serwer)
{
    const int socket_=prepare_socket(server_port, server_ip, udplite, serwer);

    *len_addr = sizeof( *serwer );
    if( bind( socket_,( struct sockaddr * ) serwer, *len_addr ) < 0 )
    {
        perror( "bind() ERROR" );
        exit( 3 );
    }
    return socket_;
}
