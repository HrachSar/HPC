#include <stdio.h>
#include <omp.h>

#define N 12

long fib(int n){
	if (n == 0)
		return 0;
	if(n == 1 || n == 2)
		return 1;

	if(n <= 10){
		int first = 0;
		int second = 1;
		int res = 0;
		for(int i = 2; i <= n; i++){
			res = first + second;
			first = second;
			second = res;
		}
		return res;
	}else{

		long f1 = 0;
		long f2 = 0;

		#pragma omp task shared(f1)
		f1 = fib(n - 1);

		#pragma omp task shared(f2)
		f2 = fib(n - 2);

		#pragma omp taskwait
		return f1 + f2;
	}
}

int main(){

	long result = 0;

	#pragma omp parallel shared(result)
	{
		#pragma omp single
		{
			result = fib(N);
		}
	}
	printf("fibonacci of %d = %ld \n", N, result);
	return 0;
}
