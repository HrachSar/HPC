#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define N 5

pthread_barrier_t barrier;

void *join_server(void *args){
	printf("Getting ready.\n");
	sleep(3);
	pthread_barrier_wait(&barrier);
	printf("Game started.\n");

	return NULL;
}


int main(){

	pthread_t threads[N];
	pthread_barrier_init(&barrier, NULL, N);

	for(int i = 0; i < N; i++){
		if(pthread_create(&threads[i], NULL, join_server, NULL) != 0){
			perror("Error on creating threads.\n");
			return 1;
		}
	}
	for(int i = 0; i < N; i++){
		if(pthread_join(threads[i], NULL) != 0){
			perror("Error on joining threads.\n");
			return 1;
		}
	}
}
