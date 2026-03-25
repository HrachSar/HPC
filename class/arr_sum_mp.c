#include <stdio.h>
#include <omp.h>

#define N 10
#define T 4

int main(){

	int sum = 0;
	int arr[N];

	for(int i = 0; i < N; i++){
		arr[i] = i + 1;
	}
    #pragma omp parallel num_threads(T)
    {
        int ID = omp_get_thread_num();
		int local_sum = 0;
		int start = ID * (N / T);
		int end;
		if (ID < T - 1)
			end = (ID + 1) * (N / T);
		else
			end = N;

		for(int i = start; i < end; i++){
			local_sum += arr[i];
		}
		sum += local_sum;
    }

	printf("Sum of the array = %d\n", sum);
    return 0;

}
