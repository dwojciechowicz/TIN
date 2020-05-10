//Autor: Magdalena Zych
//Data: 10.05.2020

#include "parameters.h"

int get_server_parameters(char *ip, int* port)
{
  FILE *file;
  if((file=fopen("parameters.txt","r"))==NULL)
  {
       perror( "fopen() ERROR" );
       exit( 5 );
  }
  fscanf(file, "%s", ip);
  fscanf(file, "%d", port);
  fclose(file);
}
