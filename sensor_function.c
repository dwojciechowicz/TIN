//sensor_function.c
//Autor: Magdalena Zych
//	 Dorota Wojciechowicz
//Data: 22.04.2020

#include "sensor_function.h"

void* sensor(void* param)
{
    struct sockaddr_in serwer;
    const int socket_=prepare_socket(server_port, server_ip, true, &serwer);

    char buffer[BUFFER_SIZE]="";
    struct sensor_parameters* parameters=(struct sensor_parameters *)param;
    srand(time(NULL));
    int r=0; // liczba pomocna przy "wysyłaniu" pakietów błędnych, lub tych co nie dotrą
    int i=0;
    while(1) //zatrzymanie działania sensorów poprzez "./stop_sensors"
    {
       	++i;
      	r = (int) rand()/(RAND_MAX+1.0)*100.0;
      	int stare;
      	if(r!=1)//zgubione pakiety
      	{
      	if (r==3)//pakiety bledne
      	{
        	stare = parameters->type;
        	parameters->type =3;
      	}
        measure(buffer, parameters->type, parameters->device_number);
        printf( "|Message nr %d for server|: ", i);
        disp_buffer(buffer);
        socklen_t len = sizeof( serwer );
      	if( sendto( socket_, &buffer, sizeof(buffer), 0,( struct sockaddr * ) & serwer, len ) < 0 )
              {
                  perror( "ERROR-sendto() \n" );
                  exit( 1 );
              }
      	if (r==3)
      	{
      	   parameters->type =stare;
      	}
      	}
        pthread_mutex_lock(&mutex_sent_packets);
        ++sent_packets;
        pthread_mutex_unlock(&mutex_sent_packets);
        sleep(parameters->sleep_time);
      }
    shutdown( socket_, SHUT_RDWR );
}

void measure(char* buffer, int sensor_type, int number)
{
    //zapis do bufora czasu pomiaru (bajty 0-7 bufora)
    writeDate(buffer);

    //typ ramki (2 bity 8 bajtu bufora), nr urzadzenia (5 bitów 8 bajtu bufora)
    //informacje te zapisywane są na kolejnym bajcie po dacie i godzinie, czyli na bajcie nr: DATE_LENGTH
    buffer[DATE_LENGTH]=((uint8_t)sensor_type<<6);
    buffer[DATE_LENGTH]=buffer[DATE_LENGTH] | ((uint8_t)number);

    //zapis do bufora wygenerowanej wartości zmierzonej (bajty 9-13 bufora)
    if(sensor_type ==0)
    writeMeasurement(buffer, sensor_type, 20.0, 1.0);
    else
    writeMeasurement(buffer, sensor_type, 70.0, 1.0);
}

void disp_buffer(char* buffer) //można by to lepiej napisać
{
    int i;
    int sign;
    for(i=0;i<BUFFER_SIZE; ++i)
    {
      sign = buffer[i];
      sign=sign & INITIAL_MASK;
      printf("%d ", sign);
    }
    printf("\n");
}

void writeDate(char* buffer)
{
  time_t t = time(NULL);
  uint64_t mask_64 = INITIAL_MASK;
  uint64_t current_byte_64=0;
  int i;
  for(i = 0; i < DATE_LENGTH; ++i)
  {
    current_byte_64=((t&mask_64)>>(BITS_IN_BYTE*(i)));
    buffer[i]=(uint8_t)current_byte_64;
    mask_64=mask_64<<BITS_IN_BYTE;
  }
}


void writeMeasurement(char* buffer, int sensor_type, float mean, float std)
{
  //generowanie liczby z rozkladu normalnego
  union bytesInterpretation measurement;
  measurement.floatValue = normalDistribution(mean, std); //potem dodać mechanizm ustalania z jakiego rozkładu ma być brana liczba
  switch(sensor_type)
  {
    case TEMP_POW: //dla temperatury dokładność co do setnych (klasyczne zaokrąglanie-od 5 w górę)
      measurement.floatValue = round(measurement.floatValue * 100) / 100;
      break;
    case WILG_POW: //dla wilgotności dokładność co do tysięcznych (klasyczne zaokrąglanie-od 5 w górę)
    case WILG_GL:
      measurement.floatValue = round(measurement.floatValue * 1000) / 1000;
      break;
    default:
      printf("Nieznany typ czujnika \n");
  }
  printf("pomiar: %f\n", measurement.floatValue);
  //wartość pomiaru (4 bajty - od 9 do 12 bajtu bufora)
  uint32_t mask_32 = INITIAL_MASK;
  uint32_t current_byte_32 = 0;
  for(int i = DATE_LENGTH + 1; i < BUFFER_SIZE; ++i)
  {
    current_byte_32=((measurement.intValue & mask_32)>>(BITS_IN_BYTE*(i-DATE_LENGTH-1)));
    buffer[i]=(uint8_t)current_byte_32;
    mask_32=mask_32<<BITS_IN_BYTE;
  }
}
