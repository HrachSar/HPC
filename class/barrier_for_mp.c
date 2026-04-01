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

		#pragma omp for nowait
		for(int i = 0; i < 4; i++){
			sleep(1);
			printf("Thread %d finished i = %d\n", ID, i);
		}

		printf("Thread %d passed the loop.\n", ID);
    }

    return 0;

}
