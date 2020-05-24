//control_mod.h
//Autor: Maria Jarek
//Data: 24.05.2020

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include "parameters.h"
#include <arpa/inet.h> // inet_pton()

void identify_action(int argc, char *argv[], int *action_type);

bool check_arguments(int l, char *args[]);
void load_params(char str[], char *args[]);

union intInBuffer  //unia potrzebna do zapisania liczby typu int do bufora
{
  int intValue;
  char buffer[5];
};
