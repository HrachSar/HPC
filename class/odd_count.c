#include <stdio.h>
#include <pthread.h>

static int counter = 0;
static int arr[10] = {1, 2, 3, 4, 5, 6, 7 ,8 ,9, 10};

typedef struct thread_args{

	int start_idx;
	int end_idx;
} thread_args;

void *count_odds(void *args){

	thread_args *targs = (thread_args *)(args);

	for(int i = targs->start_idx; i < targs->end_idx; i++){
		if(arr[i] % 2 != 0)
			counter++;
	}
	return NULL;
}


int main() {

	pthread_t threads[2];
	thread_args args[2];

	for (int i = 0; i < 2; i++) {
		args[0].start_idx = 0;
		args[0].end_idx = 10 / 2;
		args[1].start_idx = 5;
		args[1].end_idx = 10;

		if (pthread_create(&threads[i], NULL, count_odds, (void *)(&args[i])) != 0){
			perror("Error\n");
			return 1;
		}
	}

	for(int i = 0; i < 2; i++){
		if(pthread_join(threads[i], NULL) != 0){
			perror("Error\n");
			return 1;
		}
	}

	printf("The number of odd elements is %d \n", counter);
	return 0;
}
