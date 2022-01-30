#ifndef _MY_QUEUE_H
#define _MY_QUEUE_H

typedef struct node {
    struct node* next;
    int* client_socket;
} node_t;

void enqueue(int* client_socket);

int* dequeue();

#endif // _MY_MY_QUEUE_H
