
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sched.h>

#define N 10

void *tester(void *args){

	for(unsigned long long i = 0; i < 1000000000; i++);

	printf("Thread %lu is running on CPU %d.\n", pthread_self(), sched_getcpu());

	return NULL;
}


int main(){

	pthread_t threads[N];

	for(int i = 0; i < N; i++){
		if(pthread_create(&threads[i], NULL, tester, NULL) != 0){
			perror("Error on creating a thread.\n");
			return 1;
		}
	}

	for(int i = 0; i < N; i++){
		if(pthread_join(threads[i], NULL) != 0){
			perror("Error on joining a thread.\n");
			return 1;
		}
	}

	return 0;
}
