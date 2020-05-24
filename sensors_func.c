//sensors_func.c
//Autorzy: Magdalena Zych
//	   Maria Jarek
//Data: 22.04.2020

#include "sensors_func.h"

int sent_packets=0;
pthread_mutex_t mutex_sent_packets=PTHREAD_MUTEX_INITIALIZER;

bool check_sensors_arguments(char *arguments[])
{
  char *argument;
  int i, j;
  for(i=0;i<SENSOR_TYPES_NUMBER;++i)
  {
    argument=arguments[i+1];
    for(int k=0; k<strlen(argument); ++k)
    {
      if(!isdigit(argument[k]))
      {
        printf("Podany argument nie jest liczba\n");
        return false;
      }
    }

    j = atoi(argument);

    if(j>64 || j<0)
    {
        printf("Program przeznaczony na od 0 do max 64 czujników jednego rodzaju\n");
        return false;
    }
   }
   return true;
}

void* diag_server_func(void* param)
{
  char diag_server_ip[20];
  int diag_server_port;
  get_server_parameters(diag_server_ip, &diag_server_port, 3);

  struct sockaddr_in server =
  {
      .sin_family = AF_INET,
      .sin_port = htons(diag_server_port)
  };
  if( inet_pton( AF_INET, diag_server_ip, & server.sin_addr ) <= 0 )
  {
      perror( "inet_pton() ERROR" );
      exit( 1 );
  }
  const int socket_ = socket( AF_INET, SOCK_DGRAM, 0 );
  if(( socket_ ) < 0 )
  {
      perror( "socket() ERROR" );
      exit( 2 );
  }
  socklen_t server_size = sizeof(server);
  if( bind( socket_,( struct sockaddr * ) & server, server_size) < 0 )
  {
      perror( "bind() ERROR" );
      exit( 3 );
  }
  char action[ 15 ] = { };
  while( 1 )
  {
      struct sockaddr_in client = { };
      memset( action, 0, sizeof( action ) );
      if( recvfrom( socket_, action, sizeof( action ), 0,( struct sockaddr * )&client, &server_size ) < 0 )
      {
          perror( "recvfrom() ERROR" );
          exit( 4 );
      }
      if(strcmp(action, "stop")==0)  //wyłączanie czujników
      {
          struct sensor_threads_info threads_info=*(struct sensor_threads_info*)param;
          for(int i=0;i<SENSOR_TYPES_NUMBER;++i)
          {
            for(int j=0;j<threads_info.sensors_nr[i];++j)
            {
              if(pthread_cancel(*(*(threads_info.threads_table_ptr+i)+j)))
              {
                //odpowiedź że nie udało się wyłączyć czujnikow
                char buffer[5]="ERR";
                if( sendto( socket_, buffer, strlen( buffer ), 0,( struct sockaddr * ) & client, server_size ) < 0 )
                {
                    perror( "sendto() ERROR" );
                    exit( 5 );
                }
                printf( "Error- pthread_cancel (i=%d, j=%d)", i, j );
                exit(4);
              }
            }
          }
          //odpowiedź że udało się wyłączyć czujniki
          char buffer[5]="OK";
          if( sendto( socket_, buffer, strlen( buffer ), 0,( struct sockaddr * ) & client, server_size ) < 0 )
          {
              perror( "sendto() ERROR" );
              exit( 5 );
          }
      }
      else if(strcmp(action, "diag")==0) //diagnostyka
      {
        //wyslanie liczby oznaczajacej liczbe wyslanych pomiarow
        union intInBuffer int_buffer;
        pthread_mutex_lock(&mutex_sent_packets);
        int_buffer.intValue=sent_packets;
        pthread_mutex_unlock(&mutex_sent_packets);
        if( sendto( socket_, int_buffer.buffer, strlen( int_buffer.buffer ), 0,( struct sockaddr * ) & client, server_size ) < 0 )
        {
            perror( "sendto() ERROR" );
            exit( 5 );
        }
      }
      else if(check_param_communication(action)) //czy odebrany komunikat dotyczy zmiany okresu pomiarow
      {
        set_new_sleep_time(action);
      }
  }
}


bool check_param_communication(char str[])
{
  //komunikat ten sklada sie ze slowa "para" oraz 3 argumentow:
  // typu urzadzenia, nr urzadzenia oraz nowego okresu
  if(strstr(str, "para") == str && strstr(str, " ") == str+4)
	{
		return true;
	}
	return false;
}

void set_new_sleep_time(char action[])
{
  const char s[2] = " ";
  char *token;

  token = strtok(action, s);

  char * new_params[5] = { };
  for(int i=0; i<3; ++i)
  {
    new_params[i] = strtok(NULL, s);
  }
  //trzeba sprawdzic czy new_params zawiera 3 argumenty

  //trzeba dodac kontrole wartosci przekazywanych w action - czy takie czujniki sa wlaczone
  int i = atoi(new_params[0]);  //typ czujnika
  int j = atoi(new_params[1]);  //nr_id_czujnika
  int new_period = atoi(new_params[2]); //nowy okres wysylania pomiarow

  parameters[i][j].sleep_time = new_period;
}
