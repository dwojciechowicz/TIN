//control_mod.c
//Autorzy: Magdalena Zych
//         Maria Jarek
//	       Dorota Wojciechowicz
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
    struct sockaddr_in sensors_serv;
    const int socket_=prepare_socket(sensors_port, sensors_ip, false, &sensors_serv);

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

    //serwer diagnostyki
    char server_ip[20];
    int server_port;
    get_server_parameters(server_ip, &server_port, 2);
    struct sockaddr_in server;
    const int server_socket_=prepare_socket(server_port, server_ip, false, &server);
    socklen_t server_size = sizeof( server );

    //serwer odbierajacy pomiary (glowny)
    char main_server_ip[20];
    int main_server_port;
    get_server_parameters(main_server_ip, &main_server_port, 1);
    struct sockaddr_in main_server;
    const int main_server_socket_=prepare_socket(main_server_port, main_server_ip, true, &main_server);
    socklen_t main_server_size = sizeof( main_server );

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
            printf( "Wystapil blad przy probie wylaczenia czujnikow\n");

          //zatrzymanie serwera diagnostyki
          if( sendto( server_socket_, action, sizeof(action), 0,( struct sockaddr * ) & server, server_size ) < 0 )
          {
              perror( "ERROR-sendto() \n" );
              exit( 1 );
          }
          memset( buffer, 0, sizeof( buffer ) );
          if( recvfrom( server_socket_, buffer, sizeof( buffer ), 0,( struct sockaddr * ) & from, & server_size ) < 0 )
          {
              perror( "recvfrom() ERROR" );
              exit( 1 );
          }
          if(strcmp(buffer, "OK")==0)
            printf( "Wylaczono serwer diagnostyki\n");
          else
            printf( "Wystapil blad przy probie wylaczenia serwera diagnostyki\n");

          //zatrzymanie serwera odbierajacego pomiary (glownego)
          if( sendto( main_server_socket_, action, sizeof(action), 0,( struct sockaddr * ) & main_server, main_server_size ) < 0 )
          {
              perror( "ERROR-sendto() \n" );
              exit( 1 );
          }
          memset( buffer, 0, sizeof( buffer ) );
          if( recvfrom( main_server_socket_, buffer, sizeof( buffer ), 0,( struct sockaddr * ) & from, & main_server_size ) < 0 )
          {
              perror( "recvfrom() ERROR" );
              exit( 1 );
          }
          if(strcmp(buffer, "OK")==0)
            printf( "Wylaczono glowny serwer\n");
          else
            printf( "Wystapil blad przy probie wylaczenia glownego serwera\n");

          break;

      case 1:
          //odpowiedź
          memset( int_buffer.buffer, 0, sizeof( int_buffer.buffer ) );
          if( recvfrom( socket_, int_buffer.buffer, sizeof( int_buffer.buffer ), 0,( struct sockaddr * ) & from, & sensors_serv_size ) < 0 )
          {
              perror( "recvfrom() ERROR" );
              exit( 1 );
          }
          printf( "Liczba wyslanych pomiarow: %d\n", int_buffer.intValue);
          shutdown( socket_, SHUT_RDWR );

          //wyslanie zapytania do modułu odbierającego pomiary (serwera)
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
      	  for(int i=0; i<4; ++i)//odczyt lacznej liczby odebranych komunikatow
      	  {
      	     good=good+((int)(int_buffer.buffer[i] &255)<<(8*i));
      	  }

       	  for(int i=4; i<8; ++i)//odczyt liczby blednych komunikatow
      	  {
      	     wrong=wrong+((int)(int_buffer.buffer[i]&255)<<(8*(i-4)));
      	  }
                printf( "Liczba odebranych pomiarów: %d\n", good);
                printf( "Liczba odebranych błędnych pakietów: %d\n", wrong);
        break;

      case 2:
          //odpowiedz
          memset( buffer, 0, sizeof( buffer ) );
          if( recvfrom( socket_, buffer, sizeof( buffer ), 0,( struct sockaddr * ) & from, & sensors_serv_size ) < 0 )
          {
              perror( "recvfrom() ERROR" );
              exit( 1 );
          }
          if(strcmp(buffer, "OK")==0)
            printf( "Udalo sie zmienic parametr czujnika\n");
          else
            printf( "Wystapil blad przy probie zmiany parametru czujnika\n");
        break;

      default:
        printf("Nieprawidlowy typ polecenia\n");
    }

    shutdown( socket_, SHUT_RDWR );

  }
