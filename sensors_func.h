//sensors_func.h
//Autorzy: Magdalena Zych
//	   Maria Jarek
//Data: 22.04.2020

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include "sensor_function.h"
#include "parameters.h"

struct sensor_parameters **parameters;
int sensors_numbers[SENSOR_TYPES_NUMBER]; // liczba czujnkow danego typu

struct sensor_threads_info
{
  pthread_t **threads_table_ptr;
  int sensors_nr[SENSOR_TYPES_NUMBER];
};

void diag(const int socket_, struct sockaddr_in *client, socklen_t server_size);
void stop_sensors(void* param, const int socket_, struct sockaddr_in *client, socklen_t server_size);
bool check_sensors_arguments(char *arguments[]);
void change_param(char *action, const int socket_, struct sockaddr_in *client, socklen_t server_size);
void* diag_server_func(void* param);
bool check_param_communication(char str[]);
bool set_new_sleep_time(char action[]);
