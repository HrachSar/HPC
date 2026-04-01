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
    #pragma omp parallel
    {
        int ID = omp_get_thread_num();

		#pragma omp for schedule(static, 3)
		for(int i = 0; i < 8; i++){
			printf("Thread %d finished i = %d loop 1\n", ID, i);
		}
    }

    return 0;

}
