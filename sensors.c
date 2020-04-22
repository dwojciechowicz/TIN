//Autor: Magdalena Zych
//Data: 22.04.2020

#include <pthread.h>
#include "sensor_function.h"

int main(int argc, char *argv[])
{
  if(argc < SENSOR_TYPES_NUMBER+1) //zrobic jakis krótszy komunikat
  {
    printf("Poprawne wywolanie funkcji: ./sensors <liczba czujnikow mierzacych temperature powietrza>");
    printf("<liczba czujnikow mierzacych wilgotnosc powietrza> <liczba czujnikow mierzacych wilgotnosc gleby>\n");
    return 1;
  }

  //sensor_threads - table with thread reference variables
  //parametres - table with structures of sensors' parametres
  pthread_t **sensor_threads=(pthread_t **)malloc(SENSOR_TYPES_NUMBER*sizeof(pthread_t *));
  struct sensor_parametres **parametres=(struct sensor_parametres **)malloc(SENSOR_TYPES_NUMBER*sizeof(struct sensor_parametres *));
  int i, j;
  for(i=0;i<SENSOR_TYPES_NUMBER;++i)
  {
      sensor_threads[i]=(pthread_t *)malloc(atoi(argv[i+1])*sizeof(pthread_t));
      parametres[i]=(struct sensor_parametres *)malloc(atoi(argv[i+1])*sizeof(struct sensor_parametres));
  }

  for(i=0;i<SENSOR_TYPES_NUMBER;++i)
  {
    for(j=0;j<atoi(argv[i+1]);++j)
    {
      //setting sensor sensor_parametres
      parametres[i][j].type=i;
      parametres[i][j].device_number=j+1; //numeracja urządzeń od 1 ??
      if(pthread_create(*(sensor_threads+i)+j, NULL, sensor, *(parametres+i)+j)) //creating the thread
      {
        printf( "Error- pthread_create (i=%d, j=%d)", i, j );
        return 2;
      }
      if(pthread_join(*(*(sensor_threads+i)+j), NULL)) //joining
      {
        printf( "Error- pthread_join  (i=%d, j=%d)", i, j);
        return 3;
      }
    }
  }


  return 0;
}
