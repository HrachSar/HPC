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

	double min_diff = RAND_MAX;
	#pragma omp parallel
	{
		#pragma omp for reduction(min:min_diff)
		for(int i = 1; i < N; i++){
			double diff = fabs(arr[i] - arr[i - 1]);
			if(diff < min_diff) min_diff = diff;
		}
	}
	printf("Min difference = %f\n", min_diff);

	free(arr);
	return 0;
}
