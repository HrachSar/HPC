#include <stdio.h>


int main(){

	int a = 10;
	int *ptr;
	ptr = &a;

	printf("the address of ptr = %p\n", ptr); 
	printf("the address of integer = %p\n", &a); 

	*ptr = 50;	
	printf("the modified value of the integrer = %d\n", a);

	return 0;
}
