#include <stdio.h>
#include <pthread.h>
#include <math.h>


#define N 10

static float result = 0;
static float x0 = 0;
static float x = 3.14 / 4;
static int index[N];


long factorial(int i){
	long result = 1;
	if(i == 0)
		return 1;
	for(int j = 1; j <= i; j++)
		result *= j;

	return result;
}


void *calculate_series(void *args){

	int *start = (int *)args;
	//printf("%d = \n", *start);

	result += pow(-1, *start) * pow(x, 2 * (*start) + 1) / (double)factorial(2 * (*start) + 1);
	return NULL;
}

int main(){

	pthread_t threads[N];
	int start = 0;
	for(int i = 0; i < N; i++){
		index[i] = i;
		if(pthread_create(&threads[i], NULL, calculate_series, (&index[i])) != 0){
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

	printf("The taylor expansion of sin around x0 = %f for %d terms  = %f", x0, N, result);
	return 0;
}
