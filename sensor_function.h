//Autor: Magdalena Zych
//Data: 22.04.2020

#include <stdio.h>
#include <stdlib.h> // exit()
#include <unistd.h> //sleep()
#include <string.h> // memset()
#include <arpa/inet.h> // inet_pton()
#include <sys/socket.h>
#include <stdint.h>

//moje
#include <pthread.h>

#define SERWER_PORT 8080
#define SERWER_IP "192.168.1.27"
#define PERIOD 2 //czas (w sekundach) między kolejnyi pomiarami
#define BUFFER_SIZE 9 //informacja o dacie, godzinie, typie czujnika i numerze urządzenia

#define SENSOR_TYPES_NUMBER 3
#define TEMP_POW 0 //identyfikacja czujnika mierzacego temperature powietrza
#define WILG_POW 1 //identyfikacja czujnika mierzacego wilgotność powietrza
#define WILG_GL 2 //identyfikacja czujnika mierzacego wilgotność gleby

void* sensor(void* param);
void measure(char* buffer, int sensor_type, int number); //przygotowanie pomiaru
void disp_buffer(char* buffer);
struct sensor_parametres
{
  int type; //typ czujnika (TEMP_POW, WILG_POW, WILG_GL)
  int device_number;
};
