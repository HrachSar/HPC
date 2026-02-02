#include <stdio.h>
#include <stdlib.h>

int main(){

	int *ptr;

	ptr = (int *)malloc(sizeof(int));
	if(!ptr)
		exit(-1);

	*ptr = 765;
	printf("Dynamically allocated integer value = %d\n", *ptr);
	int *arr_ptr;

	arr_ptr = (int *)malloc(sizeof(int) * 5);
	if(!arr_ptr)
		exit(-1);
	for(int i = 0; i < 5; i++){
		*(arr_ptr + i) = i + 1;
		printf("%d element = %d\n", i, *(arr_ptr + i));
	}
	free(ptr);
	free(arr_ptr);
	ptr = NULL;
	arr_ptr = NULL;
	return 0;
}
