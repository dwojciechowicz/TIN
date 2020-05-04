//Autorzy: Magdalena Zych
//	   Dorota Wojciechowicz
//Data: 22.04.2020

#include <stdio.h>
#include <stdlib.h> // exit()
#include <string.h> // memset()
#include <time.h> //localtime()
#include <arpa/inet.h> // inet_pton()
#include <sys/socket.h>

#define SERWER_PORT 8080
#define SERWER_IP "192.168.1.214"
#define INITIAL_MASK 255
#define DATE_LENGTH 8 //liczba bajtów na których zapisywana jest data i godzina
#define BUFFER_SIZE 13 //informacja o dacie, godzinie, typie czujnika, numerze urządzenia i z mierzonej wartości 

union bytesInterpretation  //unia potrzebna do wykonywania operacji logicznych na bitach zmiennej typu float
{
  float floatValue;
  int intValue;
};

int main()
{
    FILE * file;
    struct sockaddr_in serwer =
    {
        .sin_family = AF_INET,
        .sin_port = htons( SERWER_PORT )
    };
    if( inet_pton( AF_INET, SERWER_IP, & serwer.sin_addr ) <= 0 )
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
	int sensor_nr = (buffer[DATE_LENGTH]&60)>>1;
	union bytesInterpretation measurement;
	measurement.intValue = 0;

	strftime( date,85,"%F.txt",timeinfo); //format yyyy-mm-dd
	strftime( hour,30,"Godzina pomiaru: %X",timeinfo); //format hh-mm-ss
	for(i=0; i<BUFFER_SIZE - DATE_LENGTH; ++i)
	{
	     measurement.intValue=measurement.intValue+((int)(buffer[i+DATE_LENGTH+1]&INITIAL_MASK)<<(8*i));
	}
	 
	if((file=fopen(date,"a"))==NULL)
	{
	     perror( "fopen() ERROR" );
	     exit( 5 );
	}
	fprintf(file,"%s ", hour);
	fprintf(file,"Typ czujnika: %d nr.%d ", sensor_type,sensor_nr);
	fprintf(file,"Pomiar: %f\n", measurement.floatValue);
	fclose(file);

        char buffer_ip[ 128 ] = { };
        printf( "|Client ip: %s port: %d|\n", inet_ntop( AF_INET, & client.sin_addr, buffer_ip, sizeof( buffer_ip ) ), ntohs( client.sin_port ) );
    }

    shutdown( socket_, SHUT_RDWR );
}
