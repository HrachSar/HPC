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
		printf(" Thread %d: phaze 1\n", ID);
		sleep(ID + 1);
		printf("Thread %d finished\n", ID);

		#pragma omp barrier

		printf(" Thread %d: phaze 2\n", ID);

    }

    return 0;

}
