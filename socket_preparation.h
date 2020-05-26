//sensors_func.h
//Autorzy: Magdalena Zych
//Data: 25.05.2020

#include <arpa/inet.h> // inet_pton()
#include <sys/socket.h>
#include <stdbool.h>
#include <stdlib.h> // exit()
#include <stdio.h>

const int prepare_socket(int server_port, char* server_ip, bool udplite, struct sockaddr_in * serwer);
const int prepare_server(socklen_t *len_addr, int server_port, char* server_ip, bool udplite, struct sockaddr_in * serwer);
