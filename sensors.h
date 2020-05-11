//Autor: Maria Jarek
//        Magdalena Zych
//Data: 06.05.2020

#include <ctype.h>
#include <pthread.h>
#include "sensor_function.h"
#include <stdbool.h>
#include "parameters.h"

bool check_arguments(char *arguments[]);
void* diag_server_func(void* arg);

struct sensor_threads_info
{
  pthread_t **threads_table_ptr;
  int sensors_nr[SENSOR_TYPES_NUMBER];
};
