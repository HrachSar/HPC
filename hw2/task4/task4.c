#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 20000000
#define N 4

unsigned long arr[SIZE];
unsigned long final_count = 0;
unsigned long seq_final_count = 0;
typedef struct thread_args{
	int start;
	int end;
	unsigned long count;
} thread_args;


void fill_array(unsigned long *arr, unsigned long size){

	for(unsigned long i = 0; i < size; i++){
		arr[i] = i + 1;
	}
}

void count_primes(unsigned long *arr, unsigned long size){

	for(unsigned long i = 1; i < size; i++){

		int is_prime = 1;
		for(unsigned long j = 2; j * j <= arr[i]; j++){
			if(arr[i] % j == 0){
				is_prime = 0;
				break;
			}
		}
		if(is_prime)
			seq_final_count++;
	}
}

void *count_primes_thread(void *args){
	thread_args *targs = (thread_args *)args;

	for(int i = targs->start; i < targs->end; i++){

		if(arr[i] < 2)
			continue;

		int is_prime = 1;
		for(unsigned long j = 2; j * j <= arr[i]; j++){
			if(arr[i] % j == 0){
				is_prime = 0;
				break;
			}
		}
		if(is_prime)
			targs->count++;
	}
}

int main(){

	pthread_t threads[N];
	thread_args args[N];
	fill_array(arr, SIZE);

	unsigned long chunck = SIZE / N;

	clock_t start, end;
	start = clock();

	for(int i = 0; i < N; i++){
		args[i].start = i * chunck;
		if(i < N - 1){
			args[i].end = (i + 1) * chunck;
		}else{
			args[i].end = SIZE;
		}
		args[i].count = 0;
		if(pthread_create(&threads[i], NULL, count_primes_thread, &args[i]) != 0){
			perror("Error on creating a thread.\n");
			return 1;
		}
	}

	for(int i = 0; i < N; i++){
		if(pthread_join(threads[i], NULL) != 0){
			perror("Error on joining a thread.\n");
			return 1;
		}
		final_count += args[i].count;
	}
	end = clock();
	printf("primes computed using 4 threads = %lu, time = %f seconds.\n", final_count, (double)(end - start) / CLOCKS_PER_SEC);

	start = clock();
	count_primes(arr, SIZE);
	end = clock();
	printf("primes computed using 1 thread = %lu, time = %f seconds.\n", seq_final_count, (double)(end - start) / CLOCKS_PER_SEC);

	return 0;
}
