#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <limits.h>
#include <pthread.h>
#include "myqueue.h"

#define SERVERPORT 8989
#define BUFSIZE 4096
#define SOCKETERROR (-1)
#define SERVER_BACKLOG 1000
#define THREAD_POOL_SIZE 20

pthread_t thread_pool[THREAD_POOL_SIZE];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

void* handle_connection(void* client_socket);
int check(int exp, const char *msg);
void* thread_function(void* arg);

int main(int argc, char *argv[])
{
    int server_socket, client_socket, addr_size;
    SA_IN server_addr, client_addr;

    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
	pthread_create(&thread_pool[i], NULL, thread_function, NULL);
    }

    check((server_socket = socket(AF_INET, SOCK_STREAM, 0)),
	  "Failed to create socket");

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVERPORT);

    check(bind(server_socket,(SA*) &server_addr, sizeof(server_addr)),
	  "Bind Failed!");

    check(listen(server_socket, SERVER_BACKLOG),
	  "Listen Failed!");

    while (true) {
	printf("Waiting for connections...\n");
	
	// wait for, and eventually accept an incoming connection
	addr_size = sizeof(SA_IN);
	check(client_socket =
	      accept(server_socket, (SA*)&client_addr, (socklen_t *)&addr_size),
	      "Accept Failed!");
	printf("Connected! \n");
	
	// do whatever we do with connection
	/* pthread_t trd; */
	/* int *pclient_socket = malloc(sizeof(int)); */
	/* *pclient_socket = client_socket; */
	/* pthread_create(&trd, NULL, handle_connection, (void *)pclient_socket); */
	int* pclient = (int*)malloc(sizeof(int));
	*pclient = client_socket;
	pthread_mutex_lock(&mutex);
	enqueue(pclient);
	pthread_cond_signal(&condition_var);
	pthread_mutex_unlock(&mutex);
        //handle_connection(client_socket);

    } // while

    close(client_socket);
    return 0;
}

void* thread_function(void* arg) {
    while (true) {
	int* pclient;
	pthread_mutex_lock(&mutex);

        if ((pclient = dequeue()) == NULL) {
	    pthread_cond_wait(&condition_var, &mutex);
	    // try again
	    pclient = dequeue();
        }
        pthread_mutex_unlock(&mutex);
	if (pclient != NULL) {
	    handle_connection(pclient);
        }
    }
}

void* handle_connection(void* pclient_socket) {
    int client_socket = *(int *)pclient_socket;
    free(pclient_socket);
    char buffer[BUFSIZE];
    size_t bytes_read;
    int msgsize = 0;
    char actualpath[PATH_MAX+1];

    // read the client's message -- the name of the file to read
    while ((bytes_read = read(client_socket, buffer + msgsize,
                              sizeof(buffer) - msgsize - 1)) > 0) {
	msgsize += bytes_read;
	if (msgsize > BUFSIZE-1 || buffer[msgsize-1] == '\n') break;
    }
    check(bytes_read, "rev error");
    buffer[msgsize-1] = 0;

    printf("REQUEST: %s\n", buffer);
    fflush(stdout);

    // validity check
    if (realpath(buffer, actualpath) == NULL) {
	printf("ERROR(bad path): %s\n", buffer);
	close(client_socket);
	return NULL; 
    }

    FILE *fp = fopen(actualpath, "r");
    if (fp == NULL) {
	printf("ERROR(open): %s\n", buffer);
	close(client_socket);
	return NULL;
    }

    while ((bytes_read = fread(buffer, 1, BUFSIZE, fp)) > 0) {
	printf("Sending %zu bytes\n", bytes_read);
	write(client_socket, buffer, bytes_read);
    }

    close(client_socket);
    fclose(fp);
    printf("closing connection\n");

    return NULL;
}

int check(int exp, const char *msg) {
  if (exp == SOCKETERROR) {
      perror(msg);
      exit(1);
  }
  return exp;
}
