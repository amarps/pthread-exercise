#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void *myTurn(void *arg) {
    int *res = (int*)malloc(sizeof(int));
    *res = 10;
    
    for (int i = 0; i < 8; ++i) {
	sleep(1);
	(*res)++;
	printf("My turn %d\n", i);
    }

    // what if we return value
    return (void*)res;
}

void *yourTurn(void *arg) {
    int *argi = (int*)arg;
    for (int i = 0; i < 8; ++i) {
	sleep(1);
	printf("Your %d turn %d\n", *argi, i);
	(*argi)++;
    }
    // what if we return value
    return NULL;
}


int main(int argc, char *argv[])
{
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, myTurn, NULL);

    int arg = 5;
    // set arg to thread
    pthread_create(&thread2, NULL, yourTurn, &arg);

    int *result;
    
    // get the return value
    pthread_join(thread1, (void*)&result);
    pthread_join(thread2, NULL);

    printf("now result is: %d\n", *result);
    printf("now arg is: %d\n", arg);
    return 0;
}

