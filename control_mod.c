//control_mod.c
//Autorzy: Maria Jarek
//	 Dorota Wojciechowicz
//Data: 24.05.2020
#include "control_mod.h"


int main(int argc, char *argv[])
{
    if(!(argc == 2 || argc == 5))
    {
      printf("Podano nieobslugiwana liczbe parametrow. Obslugiwane akcje:\n\t./control_mod diag\n\t./control_mod stop\n\t./control_mod para typ nr czas\n");
      exit( 1 );
    }

    // interpretuje podane polecenie
    int action_type; // 0 - stop, 1-diag, 2 - para -> docelowo typ enum
    identify_action(argc, argv, &action_type);

    char action[15] = { };

    switch (action_type) {
      case 0:
        strcat(action, "stop");
        break;

      case 1:
        strcat(action, "diag");
        break;

      case 2:
        if(!check_arguments(argc, argv)) exit( 1 );
        //utworzenie komunikatu dla sensors
        load_params(action, argv);
        break;

      default:
        printf("Nieprawidlowy typ polecenia\n");
    }


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

    // do case 0
    struct sockaddr_in from = { };
    char buffer[5];
    //do case 1
    union intInBuffer int_buffer;
    char server_ip[20];
    int server_port;
    socklen_t server_size;


    switch (action_type) {
      case 0:
          //odpowiedz
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
          break;

      case 1:
          //odpowiedź
          memset( int_buffer.buffer, 0, sizeof( int_buffer.buffer ) );
          if( recvfrom( socket_, int_buffer.buffer, sizeof( int_buffer.buffer ), 0,( struct sockaddr * ) & from, & sensors_serv_size ) < 0 )
          {
              perror( "recvfrom() ERROR" );
              exit( 1 );
          }
          printf( "Liczba wyslanych pomiarow: %ld\n", int_buffer.intValue);
          shutdown( socket_, SHUT_RDWR );

          //wyslanie zapytania do modułu odbierającego pomiary (serwera)
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

          server_size = sizeof( server );
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
	  int wrong=0;
	  int good=0;
	  for(int i=0; i<4; ++i)
	  {
	     good=good+((int)(int_buffer.buffer[i]&255)<<(8*i));
	  }
 	  for(int i=4; i<8; ++i)
	  {
	     wrong=wrong+((int)(int_buffer.buffer[i]&255)<<(8*i));
	  }
          printf( "Liczba odebranych pomiarow: %d\n", good);
          printf( "Liczba odebranych błędnych pakietów: %d\n", wrong);
        break;

      case 2:
        break;

      default:
        printf("Nieprawidlowy typ polecenia\n");
    }



    shutdown( socket_, SHUT_RDWR );

  }


