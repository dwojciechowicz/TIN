//control_mod.c
//Autor: Maria Jarek
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
          printf( "Liczba wyslanych pomiarow: %d\n", int_buffer.intValue);
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
          printf( "Liczba odebranych pomiarow: %d\n", int_buffer.intValue);
        break;

      case 2:
        break;

      default:
        printf("Nieprawidlowy typ polecenia\n");
    }



    shutdown( socket_, SHUT_RDWR );

  }



  void identify_action(int argc, char *argv[], int *action_type)
  {
    if(strstr(argv[1], "para") == argv[1])
  	{
  		*action_type = 2;
      return;
  	}
    if(strstr(argv[1], "diag") == argv[1])
  	{
  		*action_type = 1;
      return;
  	}
    if(strstr(argv[1], "stop") == argv[1])
  	{
  		*action_type = 0;
      return;
  	}
  }


  bool check_arguments(int l, char *args[])
  {
    if(l != 5)
    {
      printf("Podano zla liczbe argumentow. Nalezy podac typ czujnika, nr czujnika oraz nowy okres pomiaru\n");
      return false;
    }

    for(int i=2; i<5; ++i)
    {
      for(int k=0; k<strlen(args[i]); ++k)
      {
        if(!isdigit(args[i][k]))
        {
          printf("Podany argument nie jest liczba\n");
          return false;
        }
      }
    }

    //czy argumenty sa potencjalnie prawidlowymi liczbami dwucyfrowymi
    if(atoi(args[2]) < 0 || atoi(args[2]) > 2)
    {
      printf("Podano nieprawidlowy typ czujnika\n");
      return false;
    }
    if(atoi(args[3]) < 0 || atoi(args[3]) > 64)
    {
      printf("Podano nr czujnika z poza zakresu\n");
      return false;
    }
    if(atoi(args[4]) < 1 || atoi(args[4]) > 99)
    {
      printf("Podano nieobslugiwany okres pomiaru\n");
      return false;
    }

    return true;
  }

  void load_params(char str[], char *args[])
  {
    strcat(str, "para ");
    for(int i=2; i<4; ++i)
    {
      strcat(str, args[i]);
      strcat(str, " ");
    }

    //rozbite, aby ostatnim znakiem nie byla spacja ^
    strcat(str, args[4]);
  }
