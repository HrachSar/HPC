#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 50000000
#define N 10000

unsigned long arr[SIZE];
unsigned long sum = 0;
unsigned long seq_sum = 0;
typedef struct thread_args{
	int start;
	int end;
} thread_args;

void fill_array(unsigned long *arr, unsigned long size){

	for(unsigned long i = 0; i < size; i++){
		arr[i] = rand();
	}
}

void compute_sum(unsigned long *arr, unsigned long size){
	for(unsigned long i = 0; i < size; i++){
		seq_sum += arr[i];
	}
}

void *sum_array(void *args){

	unsigned long *local_sum;
	local_sum = (unsigned long *)malloc(sizeof(unsigned long));
	thread_args *targs = (thread_args *)args;

	for(unsigned long i = targs->start; i < targs->end; i++){
		*local_sum += arr[i];
	}

	return local_sum;
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

		if(pthread_create(&threads[i], NULL, sum_array, &args[i]) != 0){
			perror("Error on creating a thread.\n");
			return 1;
		}
	}

	for(int i = 0; i < N; i++){
		unsigned long *partial_sum;
		if(pthread_join(threads[i], (void **)(&partial_sum)) != 0){
			perror("Error on joining thread.\n");
			return 1;
		}
		sum += *partial_sum;
	}
	end = clock();
	printf("The sum of the array using threads = %lu\n using %f seconds.\n", sum, (double)(end - start) / CLOCKS_PER_SEC);

	start = clock();
	compute_sum(arr, SIZE);
	end = clock();
	printf("The sum of the array sequentially = %lu\n using %f seconds.\n", seq_sum, (double)(end - start) / CLOCKS_PER_SEC);

	return 0;
}
