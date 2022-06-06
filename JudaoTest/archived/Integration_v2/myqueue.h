
#ifndef MYQUEUE_H_
#define MYQUEUE_H_
#include "http_handler.h"
struct node
{
    
   struct  node * next;
   // int *client_socket; 
   HTTP_REQUEST *http_req;   
};
typedef struct  node node_t;

void enqueue(HTTP_REQUEST *http_req);
HTTP_REQUEST* dequeue();

#endif
