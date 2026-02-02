#include <stdio.h>


int main(){

	int arr[5] = {1,2,3,4,5};

	 int *ptr;
	 ptr = arr;
	 for(int i = 0; i < 5; i++){
		printf("The value of %d'th element = %d\n", i,  *(ptr + i));
	 }
	 for(int i = 0; i < 5; i++){
		*(ptr + i) *= 5;
	 }

	 for(int i = 0; i < 5; i++){
                printf("Using pointer: The value of %d'th element = %d\n", i,  *(ptr + i));
         }
	 for(int i = 0; i < 5; i++){
                printf("Using index: The value of %d'th element = %d\n", i,  arr[i]);
         }
	 return 0;
}
