//change_param.c
//Autor: Maria Jarek
//Data: 23.05.2020

#include "change_param.h"

int main(int argc, char *argv[])
{
    if(!check_arguments(argc, argv))
    {
      exit( 1 );
    }

    //utworzenie komunikatu dla sensors
    char action[15] = { };
    load_params(action, argv);

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


  bool check_arguments(int l, char *args[])
  {
    if(l != 4)
    {
      printf("Podano zla liczbe argumentow. Nalezy podac typ czujnika, nr czujnika oraz nowy okres pomiaru\n");
      return false;
    }

    for(int i=1; i<3; ++i)
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
    if(atoi(args[1]) < 0 || atoi(args[1]) > 2)
    {
      printf("Podano nieprawidlowy typ czujnika\n");
      return false;
    }
    if(atoi(args[2]) < 0 || atoi(args[2]) > 64)
    {
      printf("Podano nr czujnika z poza zakresu\n");
      return false;
    }
    if(atoi(args[3]) < 1 || atoi(args[3]) > 99)
    {
      printf("Podano nieobslugiwany okres pomiaru\n");
      return false;
    }

    return true;
  }

  void load_params(char str[], char *args[])
  {
    strcat(str, "para ");
    for(int i=1; i<3; ++i)
    {
      strcat(str, args[i]);
      strcat(str, " ");
    }

    //rozbite, aby ostatnim znakiem nie byla spacja ^
    strcat(str, args[3]);
  }
