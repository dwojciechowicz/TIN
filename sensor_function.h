//Autor: Magdalena Zych
//Data: 22.04.2020

#include <stdio.h>
#include <stdlib.h> // exit()
#include <unistd.h> //sleep()
#include <string.h> // memset()
#include <arpa/inet.h> // inet_pton()
#include <sys/socket.h>
#include <stdint.h>
#include <pthread.h>

#include "normalDistribution.h"

#define PERIOD 2 //czas (w sekundach) między kolejnyi pomiarami
#define DATE_LENGTH 8 //liczba bajtów na których zapisywana jest data i godzina
#define BUFFER_SIZE 13 //informacja o dacie, godzinie, typie czujnika, numerze urządzenia i z mierzonej wartości (4 bajty)

#define BITS_IN_BYTE 8
#define INITIAL_MASK 255

#define SENSOR_TYPES_NUMBER 3
#define TEMP_POW 0 //identyfikacja czujnika mierzacego temperature powietrza
#define WILG_POW 1 //identyfikacja czujnika mierzacego wilgotność powietrza
#define WILG_GL 2 //identyfikacja czujnika mierzacego wilgotność gleby

extern char server_ip[20];
extern int server_port;
extern int sent_packets;
extern pthread_mutex_t mutex_sent_packets;

void* sensor(void* param);
void measure(char* buffer, int sensor_type, int number); //przygotowanie pomiaru
void disp_buffer(char* buffer);
void writeDate(char* buffer);
void writeMeasurement(char* buffer, int sensor_type, float mean, float std);
struct sensor_parametres
{
  int type; //typ czujnika (TEMP_POW, WILG_POW, WILG_GL)
  int device_number;
};

union bytesInterpretation  //unia potrzebna do wykonywania operacji logicznych na bitach zmiennej typu float
{
  float floatValue;
  int intValue;
};
