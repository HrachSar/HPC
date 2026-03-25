#include <pthread.h>
#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>

#define N 8
float *buffer;
float *out;
pthread_mutex_t mutex;
int empty = 1;
int full = 0;
int print = 0;

void *produce(void *args){

	if(empty > 0){
		int i = 0;
		for(; i < N; i++){
			pthread_mutex_lock(&mutex);
			buffer[i] = i + 1;
			printf("%d production = %f ", i, buffer[i]);
			pthread_mutex_unlock(&mutex);
		}
		printf("\n");
		empty--;
		full++;
	}
	return NULL;
}

void *update_simd(void *args){

	if(full > 0){
		pthread_mutex_lock(&mutex);
		__m256 factor = _mm256_set1_ps(2.0);
		__m256 va = _mm256_load_ps(buffer);

		__m256 res = _mm256_mul_ps(va, factor);

		_mm256_storeu_ps(out, res);
		print++;
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}


void *print_buff(void *args){
	int i = 0;

	if(print > 0){
		for(; i < N; i++){
			printf("%d = %f ", i, out[i]);
		}
		printf("\n");
		pthread_mutex_lock(&mutex);
		empty++;
		full--;
		print--;
		pthread_mutex_unlock(&mutex);

	}
	return NULL;
}


int main(){

	buffer = (float *)malloc(N * sizeof(float));
	if(!buffer){
		perror("Error on allocing mem.\n");
		return 1;
	}
	out = (float *)malloc(N * sizeof(float));
	if(!out){
		perror("Error on allocing mem.\n");
		return 1;
	}

	pthread_t thread1, thread2, thread3;
	pthread_mutex_init(&mutex, NULL);

	while (1)
	{
		if(pthread_create(&thread1, NULL, produce, NULL) != 0){
			return 1;
		}
		if(pthread_create(&thread2, NULL, update_simd, NULL) != 0){
			return 1;
		}
		if(pthread_create(&thread3, NULL, print_buff, NULL) != 0){
			return 1;
		}
		if(pthread_join(thread1, NULL) != 0){
			return 1;
		}

		if(pthread_join(thread2, NULL) != 0){
			return 1;
		}

		if(pthread_join(thread3, NULL) != 0){
			return 1;
		}
	}

	free(out);
	free(buffer);
	pthread_mutex_destroy(&mutex);
	return 0;
}
