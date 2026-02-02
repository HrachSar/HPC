#include <stdio.h>

static void swap(int *a, int *b);

int main(){
	
	int a = 7;
	int b = 31;

	swap(&a, &b);
	printf("a = %d, b = %d\n", a, b);

	return 0;
}

void swap(int *a, int *b){
	int temp;

	temp = *b;
	*b = *a;
	*a = temp;
}
