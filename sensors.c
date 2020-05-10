//Autor: Magdalena Zych
//Data: 22.04.2020

#include "sensors.h"

char server_ip[20];
int server_port;

int main(int argc, char *argv[])
{
  if(argc < SENSOR_TYPES_NUMBER+1) //zrobic jakis krótszy komunikat
  {
    printf("Poprawne wywolanie funkcji: ./sensors <liczba czujnikow mierzacych temperature powietrza>");
    printf("<liczba czujnikow mierzacych wilgotnosc powietrza> <liczba czujnikow mierzacych wilgotnosc gleby>\n");
    return 1;
  }

  //wczytywanie z pliku parametres.txt adresu ip oraz portu
  get_server_parameters(server_ip, &server_port);
  if(!check_arguments(argv)) return 2;

  srand(time(NULL)); //dalej wykorzystywana bedzie funkcja rand()
  //sensor_threads - table with thread reference variables
  //parametres - table with structures of sensors' parametres
  pthread_t **sensor_threads=(pthread_t **)malloc(SENSOR_TYPES_NUMBER*sizeof(pthread_t *));
  struct sensor_parametres **parametres=(struct sensor_parametres **)malloc(SENSOR_TYPES_NUMBER*sizeof(struct sensor_parametres *));

  for(int i=0;i<SENSOR_TYPES_NUMBER;++i)
  {
      sensor_threads[i]=(pthread_t *)malloc(atoi(argv[i+1])*sizeof(pthread_t));
      parametres[i]=(struct sensor_parametres *)malloc(atoi(argv[i+1])*sizeof(struct sensor_parametres));
  }

  for(int i=0;i<SENSOR_TYPES_NUMBER;++i)
  {
    for(int j=0;j<atoi(argv[i+1]);++j)
    {
      //setting sensor sensor_parametres
      parametres[i][j].type=i;
      parametres[i][j].device_number=j; //numeracja urządzeń od 0
      if(pthread_create(*(sensor_threads+i)+j, NULL, sensor, *(parametres+i)+j)) //creating the thread
      {
        printf( "Error- pthread_create (i=%d, j=%d)", i, j );
        return 4;
      }
    }
  }

  for(int i=0;i<SENSOR_TYPES_NUMBER;++i)
  {
    for(int j=0;j<atoi(argv[i+1]);++j)
    {
      if(pthread_join(*(*(sensor_threads+i)+j), NULL)) //joining
      {
        printf( "Error- pthread_join  (i=%d, j=%d)", i, j);
        return 5;
      }
    }
  }

  //zwalnianie pamięci
  for(int i=0;i<SENSOR_TYPES_NUMBER;++i)
  {
      free(sensor_threads[i]);
      free(parametres[i]);
  }
  free(sensor_threads);
  free(parametres);

  return 0;
}

bool check_arguments(char *arguments[])
{
  char *argument;
  int i, j;
  for(i=0;i<SENSOR_TYPES_NUMBER;++i)
  {
    argument=arguments[i+1];
    for(int k=0; k<strlen(argument); ++k)
    {
      if(!isdigit(argument[k]))
      {
        printf("Podany argument nie jest liczba\n");
        return false;
      }
    }

    j = atoi(argument);

    if(j>64 || j<0)
    {
        printf("Program przeznaczony na od 0 do max 64 czujników jednego rodzaju\n");
        return false;
    }
   }
   return true;
}
