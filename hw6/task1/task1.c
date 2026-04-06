#include <stdio.h>
#include <stdlib.h>
#include <omp.h>


#define N 100000000
#define T 8

int main(){
	int *arr = (int *)malloc(N * sizeof(int));
	if(!arr){
		perror("Error on allocing memory.\n");
		return 1;
	}

	for(int i = 0; i < N; i++){
		arr[i] = rand() % 256;
	}

	int hist[256] = {0};

	#pragma omp parallel num_threads(T)
	{
		#pragma omp for
		for(int i = 0; i < N; i++){
			hist[arr[i]]++;
		}

		#pragma omp single
		{
			printf("Naive parallel version:\n");
			for(int i = 0; i < 10; i++){
				printf("First 10 values: hist[%d] = %d\n", i, hist[i]);
				hist[i] = 0;
			}
		}
	}

	#pragma omp parallel num_threads(T)
	{
		#pragma omp for
		for(int i = 0; i < N; i++){
			#pragma omp critical
			hist[arr[i]]++;
		}

		#pragma omp single
		{
			printf("Critical parallel version:\n");
			for(int i = 0; i < 10; i++){
				printf("First 10 values: hist[%d] = %d\n", i, hist[i]);
				hist[i] = 0;
			}
		}
	}

	#pragma omp parallel num_threads(T)
	{
		#pragma omp for reduction(+:hist[:256])
		for(int i = 0; i < N; i++){
			hist[arr[i]]++;
		}

		#pragma omp single
		{
			printf("Reduction parallel version:\n");
			for(int i = 0; i < 10; i++){
				printf("First 10 values: hist[%d] = %d\n", i, hist[i]);
				hist[i] = 0;
			}
		}
	}
	free(arr);
	return 0;
}
