#include <stdio.h>
#include <stdlib.h>
#include <omp.h>


#define N 20000

typedef struct log_entry{
	int request_id;
	int user_id;
	double responce_time_ms;
}
log_entry;

typedef enum TYPE{
	FAST,
	MEDIUM,
	SLOW
}
TYPE;

TYPE class_type[N];

int main(){

	log_entry *entry;

	#pragma omp parallel
	{
		#pragma omp single
		{
			entry = (log_entry *)malloc(N * sizeof(log_entry));
			if(!entry){
				perror("Error on allocating memory for entries\n");
			}
			for (int i = 0; i < N; i++){
				entry[i].request_id = i;
				entry[i].user_id = rand() % N + 1;
				entry[i].responce_time_ms = ((double)rand() / RAND_MAX) * (500 - 100);
			}
		}
		#pragma omp barrier

		#pragma omp for
		for(int i = 0; i < N; i++){
			if(entry[i].responce_time_ms < 100)
				class_type[i] = FAST;
			else if(entry[i].responce_time_ms >= 100 && entry[i].responce_time_ms < 300)
				class_type[i] = MEDIUM;
			else
				class_type[i] = SLOW;
		}

		#pragma omp barrier

		#pragma omp single
		{
			int local_counter[3] = {0};
			for(int i = 0; i < N; i++){
				if(class_type[i] == SLOW)
					local_counter[2]++;
				else if(class_type[i] == MEDIUM)
					local_counter[1]++;
				else
					local_counter[0]++;
			}

			printf("Number of slow requests = %d\n", local_counter[2]);
			printf("Number of medium requests = %d\n", local_counter[1]);
			printf("Number of fast requests = %d\n", local_counter[0]);

		}
	}
	free(entry);
	return 0;
}
