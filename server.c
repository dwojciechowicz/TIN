//server.c
//Autorzy: Magdalena Zych
//	   Dorota Wojciechowicz
//Data: 22.04.2020

#include "server_func.h"

int main()
{
    //wątek z serwerem diagnostyki
    pthread_t diag_server_thread;
    pthread_create(&diag_server_thread, NULL, diag_server_func, NULL);

    //serwer oczekujący na pomiary
    char server_ip[20];
    int server_port;
    get_server_parameters(server_ip, &server_port, 1);

    FILE * file;
    socklen_t len;

    struct sockaddr_in serwer;
    const int socket_=prepare_server(&len, server_port, server_ip, true, &serwer);

    while( 1 )
    {
        get_packet(&socket_, file, &len, diag_server_thread);
    }

    shutdown( socket_, SHUT_RDWR );
}
