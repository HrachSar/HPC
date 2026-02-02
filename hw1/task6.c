#include <stdio.h>

static int str_length(char *str);

int main(){
	
	char *str = "HPC course\0";
	char *ptr = str;
	while(*str){

		printf("%c\n", *(str));
		str++;
	}
	printf("Lenght of str = %d\n", str_length(ptr));
}

int str_length(char *str){

	if(!str)
		return 0;
	int count = 0;
	while(*str){
		count++;
		str++;
	}
	return count;
}
