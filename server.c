//server.c
//Autorzy: Magdalena Zych
//	   Dorota Wojciechowicz
//Data: 22.04.2020

#include "server.h"

int received_packets=0;
int received_wrong_packets=0;
pthread_mutex_t mutex_received_packets=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_received_wrong_packets=PTHREAD_MUTEX_INITIALIZER;

int main()
{
    //wątek z serwerem diagnostyki
    pthread_t diag_server_thread;
    pthread_create(&diag_server_thread, NULL, diag_server_func, NULL);

    //serwer oczekujący na pomiary
    char server_ip[20];
    int server_port;
    get_server_parameters(server_ip, &server_port, 1);

    FILE * file;
    struct sockaddr_in serwer =
    {
        .sin_family = AF_INET,
        .sin_port = htons( server_port )
    };
    if( inet_pton( AF_INET, server_ip, & serwer.sin_addr ) <= 0 )
    {
        perror( "inet_pton() ERROR" );
        exit( 1 );
    }

    const int socket_ = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDPLITE );
    if(( socket_ ) < 0 )
    {
        perror( "socket() ERROR" );
        exit( 2 );
    }

    char buffer[ 4096 ] = { };

    socklen_t len = sizeof( serwer );
    if( bind( socket_,( struct sockaddr * ) & serwer, len ) < 0 )
    {
        perror( "bind() ERROR" );
        exit( 3 );
    }

    while( 1 )
    {
        struct sockaddr_in client = { };

        memset( buffer, 0, sizeof( buffer ) );

        printf( "Waiting for connection...\n" );
        if( recvfrom( socket_, buffer, sizeof( buffer ), 0,( struct sockaddr * ) & client, & len ) < 0 )
        {
            perror( "recvfrom() ERROR" );
            exit( 4 );
        }

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
	     continue;
	     //exit( 5 );
	}
	int sensor_nr = buffer[DATE_LENGTH]&63;
	if(sensor_nr<0 || sensor_nr>63)
	{
	     pthread_mutex_lock(&mutex_received_wrong_packets);
	     ++received_wrong_packets;
	     pthread_mutex_unlock(&mutex_received_wrong_packets);
	     printf( "sensor_nr ERROR\n" );
	     continue;
	    // exit( 6 );
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
	     continue;
	    // exit( 7 );
	}
	else if (measurement.floatValue<0.0 || measurement.floatValue>100.0)
	{
	     pthread_mutex_lock(&mutex_received_wrong_packets);
	     ++received_wrong_packets;
	     pthread_mutex_unlock(&mutex_received_wrong_packets);
	     printf( "measurement_range ERROR\n" );
	     continue;
	   //  exit( 7 );
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

    shutdown( socket_, SHUT_RDWR );
}

void* diag_server_func(void* param)
{
  char diag_server_ip[20];
  int diag_server_port;
  get_server_parameters(diag_server_ip, &diag_server_port, 2);

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
  char action[ 10 ] = { };
  while( 1 )
  {
      struct sockaddr_in client = { };
      memset( action, 0, sizeof( action ) );
      if( recvfrom( socket_, action, sizeof( action ), 0,( struct sockaddr * )&client, &server_size ) < 0 )
      {
          perror( "recvfrom() ERROR" );
          exit( 4 );
      }

      if(strcmp(action, "diag")==0) //diagnostyka
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

	for(int i = 0; i < 4; ++i)
	{
	current_byte_32=((received_packets & mask_32)>>(8*i));
	int_buffer.buffer[i]=(uint8_t)current_byte_32;
	mask_32=mask_32<<8;
        }

	for(int i = 4; i < 8; ++i)
	{
	current_byte_32=((received_wrong_packets & mask_32)>>(8*i));
	int_buffer.buffer[i]=(uint8_t)current_byte_32;
	mask_32=mask_32<<8;
        }

        if( sendto( socket_, int_buffer.buffer, strlen( int_buffer.buffer ), 0,( struct sockaddr * ) & client, server_size ) < 0 )
        {
            perror( "sendto() ERROR" );
            exit( 5 );
        }
      }
  }
}
