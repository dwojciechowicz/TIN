//Autor: Magdalena Zych
//Data: 11.05.2020

#include <stdio.h>
#include <string.h>
#include "parameters.h"
#include <arpa/inet.h> // inet_pton()

union intInBuffer  //unia potrzebna do zapisania liczby typu int do bufora
{
  int intValue;
  char buffer[5];
};
