#include <stdio.h>
#include <pthread.h>


#define N 3


void *print_msg(void *args){

	pthread_t currID = pthread_self();
	printf("Thread %lu is running.\n", currID);

	return NULL;
}

int main(){


	pthread_t threads[N];

	for(int i = 0; i < N; i++){
		if(pthread_create(&threads[i], NULL, print_msg, NULL) != 0){
			perror("Error on creating a thread.\n");
			return 1;
		}
	}

	for(int i = 0; i < N; i++){
		if(pthread_join(threads[i], NULL) != 0){
			perror("Error on joining threads to main.\n");
			return 1;
		}
	}

	return 0;
}
