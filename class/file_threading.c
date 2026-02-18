#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define ARR_SIZE 10

char buffer[ARR_SIZE];
int out[ARR_SIZE];

typedef struct thread_args
{
	int start_index;
	int end_index;
} thread_args;


void *check_cpus(void *num_cpus){
	long lnum_cpus;

	lnum_cpus = *((long *)(num_cpus));
	lnum_cpus = sysconf(_SC_NPROCESSORS_ONLN);

	if(lnum_cpus == -1){
		perror("sysconf");
        fprintf(stderr, "Could not determine the number of CPUs, defaulting to 1.\n");
        lnum_cpus = 1;
	}
	*((long *)(num_cpus)) = lnum_cpus;
	return NULL;
}

void *square_array(void *args){
	thread_args *targs = (thread_args *)args;
	int start = targs->start_index;
	int end = targs->end_index;

	for(int i = start; i < end; i++){
		int n = buffer[i] - '0';
		n *= n;
		out[i] = n;
	}
	return NULL;
}

int main(){

	clock_t start, end;

	start = clock();
	int fd = open("text.txt", O_CREAT | O_WRONLY, 0644);
	if(fd == -1){
		perror("Failed to open/create a file.\n");
		return 1;
	}
	int i = 0;
	int wr;
	while (i < 10)
	{
		char c = i + '0';
		wr = write(fd, &c, 1);
		if(wr == -1){
			perror("Error on writing into fd.\n");
			return 1;
		}
		wr = write(fd, "\n", 1);
		if(wr == -1){
			perror("Error on writing into fd.\n");
			return 1;
		}
		i++;
	}
	close(fd);
	end = clock();

	double time_used = ((double)(end - start) / CLOCKS_PER_SEC);
	printf("time used for writing into fd(seconds) = %f\n", time_used);

	start = clock();
	fd = open("text.txt", O_RDONLY);
	int rd;
	char c;
	i = 0;
	while((rd = read(fd, &c, 1)) != 0){
			if(c != '\n'){
				buffer[i++] = c;
			}
	}
	end = clock();

	time_used = ((double)(end - start) / CLOCKS_PER_SEC);
	printf("time used for reading from fd(seconds) = %f\n", time_used);

	pthread_t thread;
	long num_cpus = 0;
	if(pthread_create(&thread, NULL, check_cpus, &num_cpus) != 0){
		perror("Error on creating a thread.\n");
		return 1;
	}

	if(pthread_join(thread, NULL) != 0){
		perror("Error on joining threads.\n");
		return 1;
	}

	printf("Number of cpus = %ld\n", num_cpus);

	pthread_t threads[num_cpus];

	int chunck = ARR_SIZE / num_cpus;
	thread_args args[num_cpus];

	start = clock();
	for(int i = 0; i < num_cpus; i++){
		args[i].start_index = i * chunck;
		if(i < num_cpus - 1){
			args[i].end_index = (i + 1) * chunck;
		}else{
			args[i].end_index = ARR_SIZE;
		}
		if(pthread_create((threads + i), NULL, square_array, &args[i]) != 0){
			perror("Error on creating a thread.\n");
			return 1;
		}
	}
	end = clock();

	for(int i = 0; i < num_cpus; i++){
		if(pthread_join(threads[i], NULL) != 0){
			perror("Error on joining threads.\n");
			return 1;
		}
	}
	time_used = ((double)(end - start) / CLOCKS_PER_SEC);
	printf("time used for processing(seconds) = %f\n", time_used);

	for(int i = 0; i < ARR_SIZE; i++)
		printf("%d\n", out[i]);

	close(fd);
}
