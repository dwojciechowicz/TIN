//Autor: Magdalena Zych
//Data: 22.04.2020

#include "sensor_function.h"

void* sensor(void* param)
{
    struct sockaddr_in serwer =
    {
        .sin_family = AF_INET,
        .sin_port = htons( SERWER_PORT )
    };
    if( inet_pton( AF_INET, SERWER_IP, & serwer.sin_addr ) <= 0 )
    {
        printf( "ERROR-inet_pton() \n" );
        exit( 1 );
    }

    const int socket_ = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDPLITE );
    if( socket_ < 0 )
    {
        printf( "ERROR-socket() \n" );
        exit( 1 );
    }

    char buffer[BUFFER_SIZE]="";
    struct sensor_parametres* parametres=(struct sensor_parametres *)param;
    //printf("Bufor na poczatku: ");
    //disp_buffer(buffer);
    int i;
    for(i=1; i<=5;i++) //liczba pomiarów każdego czujnika - docelowo tu chyba będzie while(1)
    {
        measure(buffer, parametres->type, parametres->device_number);
        printf( "|Message %d for server|: ", i);
        disp_buffer(buffer);
        socklen_t len = sizeof( serwer );
        if( sendto( socket_, &buffer, sizeof(buffer), 0,( struct sockaddr * ) & serwer, len ) < 0 )
        {
            perror( "ERROR-sendto() \n" );
            exit( 1 );
        }
        sleep(PERIOD);
      }
    shutdown( socket_, SHUT_RDWR );
}


void measure(char* buffer, int sensor_type, int number)
{
  //czas pomiaru (bajty 0-7 bufora)
    time_t t = time(NULL);
    //printf("Liczba-data do przekazania: %ld\n", t);
    uint64_t mask = 255;
    uint64_t current_byte=0;
    int i;
    for(i=7;i>=0;--i)
    {
      current_byte=((t&mask)>>(8*(7-i)));
      //printf("i= %d  maska= %lu   %ld     %ld\n", i, mask, t&mask, current_byte);
      buffer[7-i]=(uint8_t)current_byte;
      mask=mask<<8;
    }
  //typ ramki (2 bity 8 bajtu bufora)
  //nr urzadzenia (5 bitów 8 bajtu bufora)
    buffer[8]=((uint8_t)sensor_type<<6);
    buffer[8]=buffer[8] | ((uint8_t)number<<1);
}

void disp_buffer(char* buffer) //można by to lepiej napisać
{
    int i;
    int sign;
    for(i=0;i<BUFFER_SIZE; ++i)
    {
      sign = buffer[i];
      sign=sign&255;
      printf("%d ", sign);
    }
    printf("\n");
}
