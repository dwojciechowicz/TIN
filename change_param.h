//change_param.h
//Autor: Maria Jarek
//Data: 23.05.2020

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include "parameters.h"
#include <arpa/inet.h> // inet_pton()

bool check_arguments(int l, char *args[]);
void load_params(char str[], char *args[]);
