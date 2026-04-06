#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define N 50000000

int main(){
	double *arr = (double *)malloc(N * sizeof(double));
	if(!arr){
		perror("Error on allocing memory.\n");
		return 1;
	}
	srand(42);
	for(int i = 0; i < N; i++){
		arr[i] = ((double)rand());
	}

	double max_arr = arr[0];
	double T = 0;
	double sum = 0;

	#pragma omp parallel
	{
		#pragma omp for reduction(max:max_arr)
		for(int i = 1; i < N; i++){
			if(arr[i] > max_arr) max_arr = arr[i];
		}
		#pragma omp single
		{
			T = 0.8 * max_arr;
		}

		#pragma omp for reduction(+:sum)
		for(int i = 0; i < N; i++){
			if(arr[i] > T) sum += arr[i];
		}
	}
	printf("Sum of all elements > T = %f\n", sum);
	free(arr);
	return 0;
}
