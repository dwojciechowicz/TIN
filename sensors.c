//sensors.c
//Autorzy: Magdalena Zych
//	   Maria Jarek
//Data: 22.04.2020

#include "sensors_func.h"

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
  //wczytywanie z pliku parameters.txt adresu ip oraz portu
  get_server_parameters(server_ip, &server_port, 1);
  if(!check_sensors_arguments(argv)) return 2;

  srand(time(NULL)); //dalej wykorzystywana bedzie funkcja rand()
  //sensor_threads - tablica zmiennych referencyjnych watkow
  //parameters - tablica parametrow czujnikow
  pthread_t **sensor_threads=(pthread_t **)malloc(SENSOR_TYPES_NUMBER*sizeof(pthread_t *));
  parameters=(struct sensor_parameters **)malloc(SENSOR_TYPES_NUMBER*sizeof(struct sensor_parameters *));
  struct sensor_threads_info threads_info;
  threads_info.threads_table_ptr=sensor_threads;

  for(int i=0;i<SENSOR_TYPES_NUMBER;++i)
  {
      sensor_threads[i]=(pthread_t *)malloc(atoi(argv[i+1])*sizeof(pthread_t));
      parameters[i]=(struct sensor_parameters *)malloc(atoi(argv[i+1])*sizeof(struct sensor_parameters));
  }
  for(int i=0;i<SENSOR_TYPES_NUMBER;++i)
  {
    threads_info.sensors_nr[i]=atoi(argv[i+1]);

    for(int j=0;j<atoi(argv[i+1]);++j)
    {
      //ustawienie parametrow czujnika (sensor_parameters)
      parameters[i][j].type=i;
      parameters[i][j].device_number=j; //numeracja urządzeń od 0
      parameters[i][j].sleep_time = PERIOD;
      if(pthread_create(*(sensor_threads+i)+j, NULL, sensor, (void*)(*(parameters+i)+j))) //tworzenie watku
      {
        printf( "Error- pthread_create (i=%d, j=%d)", i, j );
        return 4;
      }
    }
  }

  //wątek z serwerem diagnostyki itp
  pthread_t diag_server_thread;
  pthread_create(&diag_server_thread, NULL, diag_server_func, (void *)(&threads_info));

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

  if(pthread_cancel(diag_server_thread))
  {
    printf( "Error- pthread_cancel (diag_server_thread)\n");
    return(5);
  }
  if(pthread_join(diag_server_thread, NULL)) //joining
  {
    printf( "Error- pthread_join  (diag_server_thread)");
    return 5;
  }

  //zwalnianie pamięci
  for(int i=0;i<SENSOR_TYPES_NUMBER;++i)
  {
      free(sensor_threads[i]);
      free(parameters[i]);
  }
  free(sensor_threads);
  free(parameters);

  return 0;
}

