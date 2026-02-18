#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 50000000
#define N 4

unsigned long arr[SIZE];
unsigned long final_max = 0;
unsigned long seq_final_max = 0;
typedef struct thread_args{
	int start;
	int end;
	unsigned long max;
} thread_args;

void fill_array(unsigned long *arr, unsigned long size){

	for(unsigned long i = 0; i < size; i++){
		arr[i] = rand();
	}
}

void *find_max(void *args){
	thread_args *targs = (thread_args *)args;

	unsigned long max = arr[targs->start];
	for(unsigned long i = targs->start + 1; i < targs->end; i++){
		if(max < arr[i])
			max = arr[i];
	}
	targs->max = max;
}

unsigned long arr_max(unsigned long *arr, unsigned long size){
	unsigned long max = arr[0];

	for(unsigned long i = 1; i < size; i++){
		if(max < arr[i])
			max = arr[i];
	}
	return max;
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
		args[i].max = 0;
		if(pthread_create(&threads[i], NULL, find_max, &args[i]) != 0){
			perror("Error on creating a thread.\n");
			return 1;
		}
	}

	final_max = args[0].max;
	for(int i = 0; i < N; i++){
		if(pthread_join(threads[i], NULL) != 0){
			perror("Error on joining a thread.\n");
			return 1;
		}
		if(final_max < args[i].max)
			final_max = args[i].max;
	}
	end = clock();
	printf("maximum computed using 4 threads = %lu, time = %f seconds.\n", final_max, (double)(end - start) / CLOCKS_PER_SEC);

	start = clock();
	seq_final_max = arr_max(arr, SIZE);
	end = clock();
	printf("maximum computed using 1 thread = %lu, time = %f seconds.\n", seq_final_max, (double)(end - start) / CLOCKS_PER_SEC);

	return 0;
}
