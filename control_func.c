//control_func.c
//Autorzy: Maria Jarek
//	   Krzysztof Gorlach
//Data: 24.05.2020

#include "control_func.h"


  void identify_action(int argc, char *argv[], int *action_type)
  {
    if(strstr(argv[1], "para") == argv[1])
  	{
  		*action_type = 2;
      return;
  	}
    if(strstr(argv[1], "diag") == argv[1])
  	{
  		*action_type = 1;
      return;
  	}
    if(strstr(argv[1], "stop") == argv[1])
  	{
  		*action_type = 0;
      return;
  	}
  }


  bool check_arguments(int l, char *args[])
  {
    if(l != 5)
    {
      printf("Podano zla liczbe argumentow. Nalezy podac typ czujnika, nr czujnika oraz nowy okres pomiaru\n");
      return false;
    }

    for(int i=2; i<5; ++i)
    {
      for(int k=0; k<strlen(args[i]); ++k)
      {
        if(!isdigit(args[i][k]))
        {
          printf("Podany argument nie jest liczba\n");
          return false;
        }
      }
    }

    //czy argumenty sa potencjalnie prawidlowymi liczbami dwucyfrowymi
    if(atoi(args[2]) < 0 || atoi(args[2]) > 2)
    {
      printf("Podano nieprawidlowy typ czujnika\n");
      return false;
    }
    if(atoi(args[3]) < 0 || atoi(args[3]) > 64)
    {
      printf("Podano nr czujnika z poza zakresu\n");
      return false;
    }
    if(atoi(args[4]) < 1 || atoi(args[4]) > 99)
    {
      printf("Podano nieobslugiwany okres pomiaru\n");
      return false;
    }

    return true;
  }

  void load_params(char str[], char *args[])
  {
    strcat(str, "para ");
    for(int i=2; i<4; ++i)
    {
      strcat(str, args[i]);
      strcat(str, " ");
    }

    //rozbite, aby ostatnim znakiem nie byla spacja ^
    strcat(str, args[4]);
  }
