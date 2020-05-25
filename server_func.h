//server_func.h
//Autorzy: Magdalena Zych
//	   Dorota Wojciechowicz
//Data: 25.05.2020

#include <string.h> // memset()
#include <time.h> //localtime()
#include "parameters.h"
#include <pthread.h>
#include "socket_preparation.h"

#define INITIAL_MASK 255
#define DATE_LENGTH 8 //liczba bajtów na których zapisywana jest data i godzina
#define BUFFER_SIZE 13 //informacja o dacie, godzinie, typie czujnika, numerze urządzenia i z mierzonej wartości

const int prepare_server(socklen_t *len_addr, int server_port, char* server_ip, bool udplite, struct sockaddr_in * serwer);
void process_packet(char *buffer, FILE *file, struct sockaddr_in client);
void get_packet(const int *socket_addr, FILE *file, socklen_t *len_addr, pthread_t diag_server_thread);
void send_diag(const int socket_, struct sockaddr_in *client, socklen_t len);
void get_diag_packet(const int *socket_addr, socklen_t *len_addr);
void* diag_server_func(void* arg);

union bytesInterpretation  //unia potrzebna do wykonywania operacji logicznych na bitach zmiennej typu float
{
  float floatValue;
  int intValue;
};
