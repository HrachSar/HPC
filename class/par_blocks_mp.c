#include <stdio.h>
#include <unistd.h>
#include <omp.h>


int main(){

    #pragma omp parallel num_threads(8)
    {
        int ID = omp_get_thread_num();
		printf("Printing %d thread block 1.\n", ID);
    }

	#pragma omp parallel num_threads(8)
    {
        int ID = omp_get_thread_num();
		printf("Printing %d thread block 2.\n", ID);
    }

    return 0;

}
