//parameters.h
//Autor: Magdalena Zych
//Data: 10.05.2020
#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define SERVERS_NR 3 //liczba serwerow o ktorych informacja przechowywana jest w pliku parameters.txt

bool get_server_parameters(char *ip, int* port, int server_nr);

union intInBuffer  //unia potrzebna do zapisania liczby typu int do bufora
{
  int intValue;
  char buffer[8];
};

#endif
