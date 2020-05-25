//sensors_func.h
//Autorzy: Magdalena Zych
//	   Maria Jarek
//Data: 22.04.2020

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <stdbool.h>
#include "sensor_function.h"
#include "parameters.h"

struct sensor_parameters **parameters;
int sensors_numbers[SENSOR_TYPES_NUMBER]; // liczba czujnkow danego typu

struct sensor_threads_info
{
  pthread_t **threads_table_ptr;
  int sensors_nr[SENSOR_TYPES_NUMBER];
};

union intInBuffer  //unia potrzebna do zapisania liczby typu int do bufora
{
  int intValue;
  char buffer[5];
};

bool check_sensors_arguments(char *arguments[]);
void* diag_server_func(void* arg);
bool check_param_communication(char str[]);
bool set_new_sleep_time(char action[]);
