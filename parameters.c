//parameters.c
//Autor: Magdalena Zych
//Data: 10.05.2020

#include "parameters.h"

bool get_server_parameters(char *ip, int* port, int server_nr)
{
  if(server_nr > SERVERS_NR)
    return false;

  FILE *file;
  if((file=fopen("parameters.txt","r"))==NULL)
  {
       perror( "fopen() ERROR" );
       exit( 5 );
  }
  for(int i=0; i<server_nr; ++i)
  {
    fscanf(file, "%s", ip);
    fscanf(file, "%d", port);
  }
  fclose(file);
  return true;
}
