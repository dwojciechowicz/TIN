//Autor: Dorota Wojciechowicz
//Data: 22.04.2020

#include <stdio.h>
#include <stdlib.h> // exit()
#include <string.h> // memset()
#include <time.h> //localtime()
#include <arpa/inet.h> // inet_pton()
#include <sys/socket.h>

#define SERWER_PORT 8080
#define SERWER_IP "192.168.1.215"
#define INITIAL_MASK 255
#define DATE_LENGTH 8 //liczba bajtów na których zapisywana jest data i godzina
#define BUFFER_SIZE 13 //informacja o dacie, godzinie, typie czujnika, numerze urządzenia i z mierzonej wartości 

union bytesInterpretation  //unia potrzebna do wykonywania operacji logicznych na bitach zmiennej typu float
{
  float floatValue;
  int intValue;
};
