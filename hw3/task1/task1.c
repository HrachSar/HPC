#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define S 10
#define R 10

pthread_barrier_t barrier;
pthread_mutex_t mutex;
int thread_dice[S] = {0};
int winner_count[S] = {0};


void *roll_dice(void *args){
	int *idx = (int *)args;
	int r;

	for(r = 0; r < R; r++){
		pthread_mutex_lock(&mutex);
		thread_dice[*idx] = rand() % 6 + 1;
		pthread_mutex_unlock(&mutex);
		pthread_barrier_wait(&barrier);

		if(*idx == 0){
			int max = thread_dice[*idx];
			int round_winner = *idx;
			for(int i = 1; i < S; i++){
				if(max < thread_dice[i]){
					max = thread_dice[i];
					round_winner = i;
				}
			}
			winner_count[round_winner]++;
			printf("The winner of round %d is player %d.\n", r, round_winner);
		}
	}

	return NULL;
}


int main(){
	pthread_t threads[S];
	int thread_index[S] = {0};
	pthread_barrier_init(&barrier, NULL, S);
	pthread_mutex_init(&mutex, NULL);
	int max = 0;
	int winner = 0;

	for(int i = 0; i < S; i++){
		thread_index[i] = i;
		if(pthread_create(&threads[i], NULL, roll_dice, &thread_index[i]) != 0){
			perror("Error on creating a thread.\n");
			return 1;
		}
	}
	for(int i = 0; i < S; i++){
		if(pthread_join(threads[i], NULL) != 0){
			perror("Error on joining threads.\n");
			return 1;
		}
	}

	max = winner_count[0];
	for(int k = 1; k < S; k++){
		if (max < winner_count[k]){
			max = winner_count[k];
			winner = k;
		}
	}


	pthread_mutex_destroy(&mutex);
	pthread_barrier_destroy(&barrier);

	printf("The winner of the game is player %d.\n", winner);
	return 0;
}
