#include "myqueue.h"
#include <stdlib.h>

node_t* head = NULL;
node_t* tail = NULL;

// void enqueue(int *client_socket){
//     node_t *newnode = malloc (sizeof(node_t));

//     newnode->client_socket = client_socket;
//     newnode->next = NULL;

//     if(tail == NULL)
//         head = newnode;
//     else
//         tail -> next = newnode;
//     tail = newnode;
// }


void enqueue(HTTP_REQUEST *http_req){
    node_t *newnode = malloc (sizeof(node_t));
    // int *client_socket = http_req->pclient;
    
    newnode->http_req = http_req;
    newnode->next = NULL;
    printf("queue pointer %p",head);
    if(tail == NULL)
        head = newnode;
    else
        tail -> next = newnode;
    tail = newnode;
}

//returns NULL if the queue is empty.
//returns the pointer to a client_socket, if there is one to get
HTTP_REQUEST* dequeue() {
    // If nothing to pull out from the queue, 
    // then it retuns NULL
    if(head == NULL) {
        return NULL;
    }
    else {
        HTTP_REQUEST *result = head->http_req;
        node_t *temp = head;
        head = head -> next;
        if ( head == NULL) 
            {tail = NULL;}
        free(temp);
        return result;
    }
}