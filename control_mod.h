//control_mod.h
//Autor: Maria Jarek
//Data: 24.05.2020

#include "parameters.h"
#include <arpa/inet.h> // inet_pton()
#include "control_func.h"

union intInBuffer  //unia potrzebna do zapisania liczby typu int do bufora
{
  int intValue;
  char buffer[5];
};
