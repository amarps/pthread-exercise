#include <stdio.h>
#include <pthread.h>

int counter =  0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* count_to_million(void *arg) {
  for (int i = 0; i < 50000000; i++) {
    pthread_mutex_lock(&lock);
    counter++;
    pthread_mutex_unlock(&lock);
  }
  return NULL;
}

int main(int argc, char *argv[]) {
  pthread_t thread1;
  pthread_t thread2;
  pthread_create(&thread1, NULL, count_to_million, NULL);
  pthread_join(thread1, NULL);

  pthread_create(&thread2, NULL, count_to_million, NULL);
  pthread_join(thread1, NULL);
  
  printf("res: %d", counter);
  return 0;
}
