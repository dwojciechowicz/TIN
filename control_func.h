//control_func.h
//Autorzy: Maria Jarek
//	   Krzysztof Gorlach
//Data: 24.05.2020

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

bool identify_action(int argc, char *argv[], int *action_type);

bool check_arguments(int l, char *args[]);
void load_params(char str[], char *args[]);
