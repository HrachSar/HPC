#include <stdio.h>
#include <stdlib.h>
#include <omp.h>


#define N 10000

typedef enum TYPE{
	NORMAL,
	HIGH
}
TYPE;

typedef struct order_entry{
	int order_id;
	int distance_km;
	TYPE priority;
}
order_entry;



int main(){

	int distance_threshold = 0;
	int thread_high_count[4] = {0};
	order_entry *entry;

	#pragma omp parallel num_threads(4)
	{
		#pragma omp single
		{
			entry = (order_entry *)malloc(N * sizeof(order_entry));
			if(!entry){
				perror("Error on allocating memory for entries\n");
			}
			for(int i = 0; i < N; i++){
				entry[i].order_id = i;
				entry[i].distance_km = (300) * ((double)rand() / RAND_MAX);
			}
			distance_threshold = 20;
		}
		#pragma omp for
		for(int i = 0; i < N; i++){
			if(entry[i].distance_km < distance_threshold){
				entry[i].priority = HIGH;
			}else{
				entry[i].priority = NORMAL;
			}
		}

		#pragma omp barrier

		#pragma omp single
		{
			printf("The priorities of orders are set!\n");
		}

		int id = omp_get_thread_num();
		#pragma omp for
		for(int i = 0; i < N; i++){
			if(entry[i].priority == HIGH)
				thread_high_count[id]++;
		}
		#pragma omp barrier

		#pragma omp single
		{
			int total = 0;
			for(int i = 0; i < 4; i++){
				printf("Number of high priority of orders in %d = %d\n", i, thread_high_count[i]);
				total += thread_high_count[i];
			}
			printf("Total high priority orders = %d\n", total);
		}
	}
	free(entry);
	return 0;
}
