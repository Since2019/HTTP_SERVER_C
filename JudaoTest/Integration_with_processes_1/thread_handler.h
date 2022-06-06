#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <limits.h>
#include <pthread.h>
#include "myqueue.h"

#define SERVERPORT 80
#define BUFSIZE 4096
#define SOCKETERROR (-1)
// #define SERVER_BACKLOG 50

void * handle_connection(void* p_client_socket);

// void * thread_function(void* arg);

int check(int exp, const char *msg);
