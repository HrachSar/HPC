#include <stdio.h>
#include <pthread.h>
#include <immintrin.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <inttypes.h>

#define N 100000000
#define T 16

char *buffer;
unsigned long count[4] = {0};
pthread_mutex_t mutex;

typedef struct thread_args{
	size_t start;
	size_t end;
}
thread_args;

typedef struct timer{
	double time_sc;
	double time_mt;
	double time_simd;
	double time_simd_mt;
}timer;

void reset_count(unsigned long *count){
	for(int i = 0; i < 4; i++)
		count[i] = 0;
}
void print_count(unsigned long *count){
	for(int i = 0; i < 4; i++){
		printf("count[%d] = %ld \n", i, count[i]);
	}
}

void generate_dna(char *buffer, size_t size){
	char types[4] = {'A','C','G','T'};

	for(size_t i = 0; i < size; i++){
		buffer[i] = types[rand() % 4];
	}
}

void *process_dna_mt(void *args){

	thread_args *targs = ((thread_args *)args);
	unsigned long local_counter[4] = {0};

	for(size_t i = targs->start; i < targs->end; i++){
		if(buffer[i] == 'A'){
			local_counter[0]++;
		}else if(buffer[i] == 'C'){
			local_counter[1]++;
		}else if(buffer[i] == 'G'){
			local_counter[2]++;
		}else{
			local_counter[3]++;
		}
	}
	pthread_mutex_lock(&mutex);
	count[0] += local_counter[0];
	count[1] += local_counter[1];
	count[2] += local_counter[2];
	count[3] += local_counter[3];
	pthread_mutex_unlock(&mutex);

	return NULL;
}

void process_dna_scalar(){
	for(size_t i = 0; i < N; i++){
		if(buffer[i] == 'A'){
			count[0]++;
		}else if(buffer[i] == 'C'){
			count[1]++;
		}else if(buffer[i] == 'G'){
			count[2]++;
		}else{
			count[3]++;
		}
	}
}

void process_dna_simd(){
	size_t i = 0;
	__m256i a = _mm256_set1_epi8('A');
	__m256i c = _mm256_set1_epi8('C');
	__m256i g = _mm256_set1_epi8('G');
	__m256i t = _mm256_set1_epi8('T');

	for(; i < N; i += 32){
		__m256i va = _mm256_loadu_si256((__m256i *)&buffer[i]);
		__m256i m1 = _mm256_cmpeq_epi8(va, a);
		__m256i m2 = _mm256_cmpeq_epi8(va, c);
		__m256i m3 = _mm256_cmpeq_epi8(va, g);
		__m256i m4 = _mm256_cmpeq_epi8(va, t);

		int mask1 = _mm256_movemask_epi8(m1);
		int mask2 = _mm256_movemask_epi8(m2);
		int mask3 = _mm256_movemask_epi8(m3);
		int mask4 = _mm256_movemask_epi8(m4);

		count[0] += __builtin_popcount(mask1);
		count[1] += __builtin_popcount(mask2);
		count[2] += __builtin_popcount(mask3);
		count[3] += __builtin_popcount(mask4);

	}
}

void *process_dna_mt_simd(void *args){
	thread_args *targs = ((thread_args *)args);

	size_t i = targs->start;
	__m256i a = _mm256_set1_epi8('A');
	__m256i c = _mm256_set1_epi8('C');
	__m256i g = _mm256_set1_epi8('G');
	__m256i t = _mm256_set1_epi8('T');

	size_t limit = targs->start + ((targs->end - targs->start)  & ~(32 - 1));
	unsigned long local_counter[4] = {0};

	for(; i < limit; i += 32){
		__m256i va = _mm256_loadu_si256((__m256i *)&buffer[i]);
		__m256i m1 = _mm256_cmpeq_epi8(va, a);
		__m256i m2 = _mm256_cmpeq_epi8(va, c);
		__m256i m3 = _mm256_cmpeq_epi8(va, g);
		__m256i m4 = _mm256_cmpeq_epi8(va, t);

		int mask1 = _mm256_movemask_epi8(m1);
		int mask2 = _mm256_movemask_epi8(m2);
		int mask3 = _mm256_movemask_epi8(m3);
		int mask4 = _mm256_movemask_epi8(m4);

		local_counter[0] += __builtin_popcount(mask1);
		local_counter[1] += __builtin_popcount(mask2);
		local_counter[2] += __builtin_popcount(mask3);
		local_counter[3] += __builtin_popcount(mask4);
	}

	for(; i < targs->end; i++){
		if(buffer[i] == 'A'){
			local_counter[0]++;
		}else if(buffer[i] == 'C'){
			local_counter[1]++;
		}else if(buffer[i] == 'G'){
			local_counter[2]++;
		}else{
			local_counter[3]++;
		}
	}
	pthread_mutex_lock(&mutex);
	count[0] += local_counter[0];
	count[1] += local_counter[1];
	count[2] += local_counter[2];
	count[3] += local_counter[3];
	pthread_mutex_unlock(&mutex);

	return NULL;
}

int main(){

	srand(42);
	buffer = (char *)malloc(N);
	if(!buffer){
		perror("Error on allocing mem.\n");
		return 1;
	}
	generate_dna(buffer, N);

    pthread_t threads[T];
	thread_args args[T];
	timer t;
	struct timespec start, end;

    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        perror("clock_gettime error");
        return 1;
    }
	pthread_mutex_init(&mutex, NULL);

	for(int i = 0; i < T; i++){

		args[i].start = i * (N / T);
		if(i < T - 1){
			args[i].end = (i + 1) * (N / T);
		}else{
			args[i].end = N;
		}
		if(pthread_create(&threads[i], NULL, process_dna_mt, &args[i]) != 0){
			perror("Error on creating threads.\n");
			return 1;
		}
	}

	for(int i = 0; i < T; i++){
		if(pthread_join(threads[i], NULL) != 0){
			perror("Error on joining threads.\n");
			return 1;
		}
	}
	if (clock_gettime(CLOCK_MONOTONIC, &end) == -1) {
        perror("clock_gettime error");
        return 1;
    }

	t.time_mt = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
	reset_count(count);


	if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        perror("clock_gettime error");
        return 1;
    }
	process_dna_scalar();
	if (clock_gettime(CLOCK_MONOTONIC, &end) == -1) {
        perror("clock_gettime error");
        return 1;
    }
	t.time_sc = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

	reset_count(count);

	if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        perror("clock_gettime error");
        return 1;
    }
	process_dna_simd();
	if (clock_gettime(CLOCK_MONOTONIC, &end) == -1) {
        perror("clock_gettime error");
        return 1;
    }
	t.time_simd = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
	reset_count(count);

	if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        perror("clock_gettime error");
        return 1;
    }
	for(int i = 0; i < T; i++){

		args[i].start = i * (N / T);
		if(i < T - 1){
			args[i].end = (i + 1) * (N / T);
		}else{
			args[i].end = N;
		}
		if(pthread_create(&threads[i], NULL, process_dna_mt_simd, &args[i]) != 0){
			perror("Error on creating threads.\n");
			return 1;
		}
	}

	for(int i = 0; i < T; i++){
		if(pthread_join(threads[i], NULL) != 0){
			perror("Error on joining threads.\n");
			return 1;
		}
	}
	if (clock_gettime(CLOCK_MONOTONIC, &end) == -1) {
        perror("clock_gettime error");
        return 1;
    }
	t.time_simd = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;


	printf("DNA SIZE: %d MB\n", N / (1000 * 1000));
	printf("Threads used: %d\n", T);
	printf("\n");
	printf("Counts (A C G T):\n");
	print_count(count);
	printf("\n");
	printf("Scalar time = %f\n", t.time_sc);
	printf("Multithreading time = %f\n", t.time_mt);
	printf("SIMD time = %f\n", t.time_simd);
	printf("SIMD + Multithreading time = %f\n", t.time_simd_mt);

	pthread_mutex_destroy(&mutex);
	free(buffer);
	return 0;
}
