#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define S 10


pthread_barrier_t barrier;
int thread_data[S] = {0};
float avg = 0;


void *collect_data(void *args){
	int *idx = (int *)args;

	thread_data[*idx] = rand() % 200;
	int status = pthread_barrier_wait(&barrier);

	if(status = PTHREAD_BARRIER_SERIAL_THREAD){
		for(int i = 0; i < S; i++){
			avg += thread_data[i];
		}
		avg /= S;
	}
	return NULL;
}


int main(){
	pthread_t threads[S];
	int thread_index[S] = {0};
	pthread_barrier_init(&barrier, NULL, S);

	for(int i = 0; i < S; i++){
		thread_index[i] = i;
		if(pthread_create(&threads[i], NULL, collect_data, &thread_index[i]) != 0){
			perror("Error on creating a thread.\n");
			return 1;
		}
	}
	for(int i = 0; i < S; i++){
		if(pthread_join(threads[i], NULL) != 0){
			perror("Error on joining threads.\n");
			return 1;
		}
	}

	printf("The average temperature is %f.\n", avg);
	pthread_barrier_destroy(&barrier);

	return 0;
}
