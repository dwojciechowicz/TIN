//server_func.c
//Autorzy: Magdalena Zych
//	   Dorota Wojciechowicz
//Data: 25.05.2020

#include "server_func.h"

bool stop=false; //parametr oznaczajacy czy nalezy zatrzymac serwery
int received_packets=0;
int received_wrong_packets=0;
pthread_mutex_t mutex_stop=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_received_packets=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_received_wrong_packets=PTHREAD_MUTEX_INITIALIZER;

void process_packet(char *buffer, FILE *file, struct sockaddr_in client)
{
    pthread_mutex_lock(&mutex_received_packets);
    ++received_packets;
    pthread_mutex_unlock(&mutex_received_packets);

    time_t t = 0;
    int i;
    for(i=0; i<DATE_LENGTH; ++i)
    {
        t=t+((uint64_t)(buffer[i]&INITIAL_MASK)<<(8*i));
    }

    struct tm *timeinfo;
    timeinfo = localtime(&t);

    char date[85];
    char hour[30];
    int sensor_type = (buffer[DATE_LENGTH]&192)>>6;
    if (sensor_type==3)
    {
         pthread_mutex_lock(&mutex_received_wrong_packets);
         ++received_wrong_packets;
         pthread_mutex_unlock(&mutex_received_wrong_packets);
         printf( "sensor_type ERROR\n" );
         return;
    }
    int sensor_nr = buffer[DATE_LENGTH]&63;
    if(sensor_nr<0 || sensor_nr>63)
    {
         pthread_mutex_lock(&mutex_received_wrong_packets);
         ++received_wrong_packets;
         pthread_mutex_unlock(&mutex_received_wrong_packets);
         printf( "sensor_nr ERROR\n" );
         return;
    }
    union bytesInterpretation measurement;
    measurement.intValue = 0;

    strftime( date,85,"%F.txt",timeinfo); //format yyyy-mm-dd
    strftime( hour,30,"Godzina pomiaru: %X",timeinfo); //format hh-mm-ss
    for(i=0; i<BUFFER_SIZE - DATE_LENGTH; ++i)
    {
         measurement.intValue=measurement.intValue+((int)(buffer[i+DATE_LENGTH+1]&INITIAL_MASK)<<(8*i));
    }
    if(sensor_nr == 0 && (measurement.floatValue<-50.0 || measurement.floatValue>80.0))
    {
         pthread_mutex_lock(&mutex_received_wrong_packets);
         ++received_wrong_packets;
         pthread_mutex_unlock(&mutex_received_wrong_packets);
         printf( "measurement_range ERROR\n" );
         return;
    }
    else if (measurement.floatValue<0.0 || measurement.floatValue>100.0)
    {
         pthread_mutex_lock(&mutex_received_wrong_packets);
         ++received_wrong_packets;
         pthread_mutex_unlock(&mutex_received_wrong_packets);
         printf( "measurement_range ERROR\n" );
         return;
    }
    if((file=fopen(date,"a"))==NULL)
    {
         perror( "fopen() ERROR" );
         exit( 8 );
    }
    fprintf(file,"%s ", hour);
    fprintf(file,"Typ czujnika: %d nr.%d ", sensor_type,sensor_nr);
    if(sensor_nr == 0)
        { fprintf(file,"Pomiar: %.3f\n", measurement.floatValue);}
    else
        { fprintf(file,"Pomiar: %.2f\n", measurement.floatValue);}
    fclose(file);
    char buffer_ip[ 128 ] = { };
    printf( "|Client ip: %s port: %d|\n", inet_ntop( AF_INET, & client.sin_addr, buffer_ip, sizeof( buffer_ip ) ), ntohs( client.sin_port ) );
}

void get_packet(const int *socket_addr, FILE *file, socklen_t *len_addr, pthread_t diag_server_thread)
{
    socklen_t len=*len_addr;
    const int socket_=*socket_addr;
    char buffer[ 4096 ] = { };
    struct sockaddr_in client = { };

    memset( buffer, 0, sizeof( buffer ) );

    printf( "Waiting for connection...\n" );
    if( recvfrom( socket_, buffer, sizeof( buffer ), 0,( struct sockaddr * ) & client, & len ) < 0 )
    {
        perror( "recvfrom() ERROR" );
        exit( 4 );
    }

    pthread_mutex_lock(&mutex_stop);
    if(stop) //nalezy zatrzymac serwer
    {
      pthread_join(diag_server_thread, NULL);
      printf("Wylaczanie glownego serwera\n");
      char buffer[5]="OK";
      // wyslanie potwierdzenia
      if( sendto( socket_, buffer, sizeof( buffer ), 0,( struct sockaddr * ) &client, len ) < 0 )
      {
          perror( "sendto() ERROR" );
          exit( 5 );
      }
      pthread_exit(NULL);
    }
    pthread_mutex_unlock(&mutex_stop);

    process_packet(buffer, file, client);
}

void send_diag(const int socket_, struct sockaddr_in *client, socklen_t len)
{
    //wyslanie liczby oznaczajacej liczbe odebranych pomiarow
    union intInBuffer int_buffer;
    uint32_t mask_32 = 255;
    uint32_t current_byte_32 = 0;

    pthread_mutex_lock(&mutex_received_packets);
    int dobre = received_packets;
    pthread_mutex_unlock(&mutex_received_packets);

    pthread_mutex_lock(&mutex_received_wrong_packets);
    int zle = received_wrong_packets;
    pthread_mutex_unlock(&mutex_received_wrong_packets);

    for(int i = 0; i < 4; ++i)//zapis lacznej liczby odebranych komunikatow
    {
      current_byte_32=((dobre & mask_32)>>(8*i));
      int_buffer.buffer[i]=(uint8_t)current_byte_32;
      mask_32=mask_32<<8;
    }

    mask_32=255;
    for(int i = 4; i < 8; ++i)//zapis liczby blednych komunikatow
    {
      current_byte_32=((zle & mask_32)>>(8*(i-4)));
      int_buffer.buffer[i]=(uint8_t)current_byte_32;
      mask_32=mask_32<<8;
    }

    if( sendto( socket_, int_buffer.buffer, sizeof( int_buffer.buffer ), 0,( struct sockaddr * ) client, len ) < 0 )
    {
        perror( "sendto() ERROR" );
        exit( 5 );
    }
}

void get_diag_packet(const int *socket_addr, socklen_t *len_addr)
{
  socklen_t len=*len_addr;
  const int socket_=*socket_addr;
  struct sockaddr_in client = { };
  char action[ 10 ] = { };
  memset(action, 0, sizeof( action ));

  if( recvfrom( socket_, action, sizeof( action ), 0,( struct sockaddr * )&client, &len ) < 0 )
  {
      perror( "recvfrom() ERROR" );
      exit( 4 );
  }

  if(strcmp(action, "diag")==0) //diagnostyka
  {
      send_diag(socket_, &client, len);
  }
  else if(strcmp(action, "stop")==0) //zatrzymywanie serwera
  {
    printf("Wylaczenie serwera diagnostycznego\n");
    pthread_mutex_lock(&mutex_stop);
    stop=true;
    pthread_mutex_unlock(&mutex_stop);
    char buffer[5]="OK";
    // wyslanie potwierdzenia
    if( sendto( socket_, buffer, sizeof( buffer ), 0,( struct sockaddr * ) &client, len ) < 0 )
    {
        perror( "sendto() ERROR" );
        exit( 5 );
    }
    pthread_exit(NULL);
  }
}

void* diag_server_func(void* param)
{
  char server_ip[20];
  int server_port;
  get_server_parameters(server_ip, &server_port, 2);
  socklen_t len;

  struct sockaddr_in server;

  const int socket_=prepare_server(&len, server_port, server_ip, false, &server);

  while( 1 )
  {
    get_diag_packet(&socket_, &len);
  }
}
