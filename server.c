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
    socklen_t len;

    const int socket_=prepare_server(&len, server_port, server_ip, true);

    while( 1 )
    {
        get_packet(&socket_, file, &len);
    }

    shutdown( socket_, SHUT_RDWR );
}

const int prepare_server(socklen_t *len_addr, int server_port, char* server_ip, bool udplite)
{
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

    *len_addr = sizeof( serwer );
    if( bind( socket_,( struct sockaddr * ) & serwer, *len_addr ) < 0 )
    {
        perror( "bind() ERROR" );
        exit( 3 );
    }
    return socket_;
}

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

void get_packet(const int *socket_addr, FILE *file, socklen_t *len_addr)
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
}

void* diag_server_func(void* param)
{
  char server_ip[20];
  int server_port;
  get_server_parameters(server_ip, &server_port, 2);
  socklen_t len;
  const int socket_=prepare_server(&len, server_port, server_ip, false);

  while( 1 )
  {
    get_diag_packet(&socket_, &len);
  }
}
