#include <stdio.h>


int main(){

	char **mat;
	char **ptr;
	mat[0] = "HPC";
	mat[1] = "Course";
	mat[2] = "has";
	mat[3] = "begun";
	int i;
	ptr = mat;
	i = 0;
	
	while(i++ < 4){
		printf("%s\n", *mat);
		mat++;
	}	
	ptr[2] = "has not";
	puts("\n");
	i = 0;
	
	while(i++ < 4){
		printf("%s\n", *ptr);
		ptr++;
	}
	return 0;
}
