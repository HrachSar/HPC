#include <stdio.h>
#include <pthread.h>

#define M 5
#define S 3

pthread_barrier_t barrier;

void *process_pipeline(void *args){

	int s;
	for(s = 0; s < 3; s++){
		printf("Stage %d is complete.\n", s + 1);
		pthread_barrier_wait(&barrier);
	}

	return NULL;
}

int main(){
	pthread_t threads[M];
	pthread_barrier_init(&barrier, NULL, M);

	for(int i = 0; i < M; i++){
		if(pthread_create(&threads[i], NULL, process_pipeline, NULL) != 0){
			perror("Error on creating a thread.\n");
			return 1;
		}
	}
	for(int i = 0; i < M; i++){
		if(pthread_join(threads[i], NULL) != 0){
			perror("Error on joining threads.\n");
			return 1;
		}
	}

	return 0;
}
