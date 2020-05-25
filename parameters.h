//parameters.h
//Autor: Magdalena Zych
//Data: 10.05.2020

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define SERVERS_NR 3 //liczba serwerow o ktorych informacja przechowywana jest w pliku parameters.txt

bool get_server_parameters(char *ip, int* port, int server_nr);
