#include <stdio.h>


int main(){

	int a = 10;
	int *ptr = &a;

	int **double_ptr = &ptr;

	printf("Using integer: %d\n", a);
	printf("Using pointer: %d\n", *ptr);
	printf("Using double-pointer: %d\n", **double_ptr);

	return 0;

}

