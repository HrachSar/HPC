#include <stdio.h>
#include <unistd.h>
#include <omp.h>

#define N 10
#define T 4

int main(){

	int sum = 0;
	int arr[N];

	for(int i = 0; i < N; i++){
		arr[i] = i + 1;
	}
    #pragma omp parallel num_threads(8)
    {
        int ID = omp_get_thread_num();
		printf("Printing %d thread.\n", ID);

		#pragma omp single
		{
			printf("Master thread (id = %d) is doing special work.\n", ID);
		}

		printf("Thread %d continues\n", ID);
    }

    return 0;

}
